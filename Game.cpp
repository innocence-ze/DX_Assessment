//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


//toreorganise
#include <fstream>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game()
{
	m_Sound->Shutdown();
	m_SoundClick->Shutdown();
#ifdef DXTK_AUDIO
	if (m_audEngine)
	{
		m_audEngine->Suspend();
	}
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{

	m_input.Initialise(window);

	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	m_fullscreenRect.left = 0;
	m_fullscreenRect.top = 0;
	m_fullscreenRect.right = 800;
	m_fullscreenRect.bottom = 600;

	m_CameraViewRect.left = 500;
	m_CameraViewRect.top = 0;
	m_CameraViewRect.right = 800;
	m_CameraViewRect.bottom = 240;

	//setup light
	m_Light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.setPosition(2.0f, 1.0f, 1.0f);
	m_Light.setDirection(-1.0f, -1.0f, 0.0f);

	//setup camera
	m_Camera01.setPosition(Vector3(0.0f, 0.0f, 0.0f));
	m_Camera01.setRotation(Vector3(-90.0f, -180.0f, 0.0f));	//orientation is -90 becuase zero will be looking up at the sky straight up. 

	m_Camera02.setPosition(Vector3(0.0f, 30.0f, 0.0f));
	m_Camera02.setRotation(Vector3(-180.0f, 0.0f, 0.0f));


	m_Sound = new Sound;
	bool result = m_Sound->Initialize(window, "musicmono_adpcm.wav", true);
	if (!result)
	{
		MessageBox(window, L"Couldn't initialize Direct sound", L"Error", MB_OK);
	}

	m_SoundClick = new Sound;
	result = m_SoundClick->Initialize(window, "sound01.wav", false);
	m_SoundClick->PauseWaveFile();
	if (!result)
	{
		MessageBox(window, L"Couldn't initialize Direct sound", L"Error", MB_OK);
	}


#ifdef DXTK_AUDIO
	// Create DirectXTK for Audio objects
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif

	m_audEngine = std::make_unique<AudioEngine>(eflags);

	m_audioEvent = 0;
	m_audioTimerAcc = 10.f;
	m_retryDefault = false;

	m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

	m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
	m_effect1 = m_soundEffect->CreateInstance();
	m_effect2 = m_waveBank->CreateInstance(10);

	m_effect1->Play(true);
	m_effect2->Play();
#endif
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
	//take in input
	m_input.Update();								//update the hardware
	m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game

	//Update all game objects
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	//Render all game content. 
	Render();

#ifdef DXTK_AUDIO
	// Only update audio engine once per frame
	if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
	{
		// Setup a retry in 1 second
		m_audioTimerAcc = 1.f;
		m_retryDefault = true;
	}
#endif


}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	MainCameraControlUpdate(timer);
	m_Camera02.Update();
	double t = timer.GetTotalSeconds();
	lerpTimer = fmod(t, 10.0) / 10;

	m_ParticleSystem.Frame(timer.GetElapsedSeconds(), m_deviceResources->GetD3DDeviceContext());

	m_world = Matrix::Identity;

#ifdef DXTK_AUDIO
	m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
	if (m_audioTimerAcc < 0)
	{
		if (m_retryDefault)
		{
			m_retryDefault = false;
			if (m_audEngine->Reset())
			{
				// Restart looping audio
				m_effect1->Play(true);
			}
		}
		else
		{
			m_audioTimerAcc = 4.f;

			m_waveBank->Play(m_audioEvent++);

			if (m_audioEvent >= 11)
				m_audioEvent = 0;
		}
	}
#endif

	if (m_input.GetMouseButtonDown(0))
	{
		m_SoundClick->PlayWaveFile();
	}


	if (m_input.Quit())
	{
		ExitGame();
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	m_deviceResources->PIXBeginEvent(L"Render");
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();

	// Draw Text to the screen
	m_sprites->Begin();
	m_font->DrawString(m_sprites.get(), L"DirectXTK Demo Window", XMFLOAT2(10, 10), Colors::Yellow);
	m_sprites->End();

	//Set Rendering states. 
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
	context->RSSetState(m_states->CullClockwise());
	if(m_gameInputCommands.wireframe)
		context->RSSetState(m_states->Wireframe());

	//create our render to texture.
	RenderTexturePass1();

	/////////////////////////////////////////////////////////////draw skybox
	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix skyboxScale = XMMatrixScaling(30.0f, 30.0f, 30.0f);
	SimpleMath::Matrix skyboxTranslation = XMMatrixTranslation(XMVectorGetX(m_Camera01.getPosition()), XMVectorGetY(m_Camera01.getPosition()), XMVectorGetZ(m_Camera01.getPosition()));
	m_world = skyboxScale * skyboxTranslation;
	m_SkyBoxShaderPair.EnableShader(context);
	m_SkyBoxShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_skyboxTexture.Get());

	m_SkyBox->Render(context);

	/////////////////////////////////////////////////////////////draw our scene normally. 
	m_world = SimpleMath::Matrix::Identity;
	m_world = m_world * SimpleMath::Matrix::CreateTranslation(0, 3, 0);
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get());
	m_ModelSphere.Render(context);

	////prepare transform for statue. 
	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix newPosition1 = SimpleMath::Matrix::CreateTranslation(5.0f, -0.7f, 0.0f);
	SimpleMath::Matrix newScale1 = SimpleMath::Matrix::CreateScale(1.5f, 1.5f, 1.5f);
	SimpleMath::Quaternion rot1 = SimpleMath::Quaternion::CreateFromYawPitchRoll(a2r * -90, 0, 0);
	m_world = m_world * SimpleMath::Matrix::CreateFromQuaternion(rot1) * newScale1* newPosition1;
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture2.Get());
	m_ModelStatue.Render(context);

	//prepare transform for floor object. 
	m_world = SimpleMath::Matrix::Identity; //set world back to identity
	SimpleMath::Matrix newPosition3 = SimpleMath::Matrix::CreateTranslation(0.0f, -0.6f, 0.0f);
	m_world = m_world * newPosition3;
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture1.Get());
	m_ModelFloor.Render(context);

	//prepare for stone.
	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix newScale4 = SimpleMath::Matrix::CreateScale(0.5f, 0.5f, 0.5f);
	SimpleMath::Matrix newPosition4 = SimpleMath::Matrix::CreateTranslation(-5, -0.7f, 0);
	SimpleMath::Quaternion rot4 = SimpleMath::Quaternion::CreateFromYawPitchRoll(a2r * 90, 0, 0);
	m_world = m_world * Matrix::CreateFromQuaternion(rot4) * newScale4 * newPosition4;
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture3.Get());
	m_ModelStone.Render(context);

	//prepare for EvilDrone
	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix newPosition5 = SimpleMath::Matrix::CreateTranslation(0, 3, 7);
	SimpleMath::Quaternion oldRotation5 = SimpleMath::Quaternion::CreateFromYawPitchRoll(a2r * 30, a2r*40, a2r * 20);
	SimpleMath::Quaternion targetRotation5 = SimpleMath::Quaternion::CreateFromYawPitchRoll(a2r * 90, a2r * 90, a2r * 80);

	SimpleMath::Quaternion newRotation5 = SimpleMath::Quaternion::Slerp(oldRotation5, targetRotation5, lerpTimer);
	m_world = m_world * SimpleMath::Matrix::CreateTranslation(0, 7, 0) * SimpleMath::Matrix::CreateFromQuaternion(newRotation5) * newPosition5;
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_texture4.Get());
	m_ModelEvilDrone.Render(context);

	////for particle system
	//m_world = SimpleMath::Matrix::Identity;
	//m_ParticleSystemShaderPair.EnableShader(context);
	//m_ParticleSystemShaderPair.SetShaderParameters(context, &m_world, &m_view, &m_projection, &m_Light, m_particlesystemTexture.Get());
	//m_ParticleSystem.Render(context);
	///////////////////////////////////////draw our sprite with the render texture displayed on it. 
	m_sprites->Begin();
	m_sprites->Draw(m_FirstRenderPass->getShaderResourceView(), m_CameraViewRect);
	m_sprites->End();

	// Show the new frame.
	m_deviceResources->Present();
}

void Game::RenderTexturePass1()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTargetView = m_deviceResources->GetRenderTargetView();
	auto depthTargetView = m_deviceResources->GetDepthStencilView();
	// Set the render target to be the render to texture.
	m_FirstRenderPass->setRenderTarget(context);
	// Clear the render to texture.
	m_FirstRenderPass->clearRenderTarget(context, 0.8f, 0.8f, 0.8f, 0.8f);

	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix miniCamPosition = SimpleMath::Matrix::CreateTranslation(m_Camera01.getPosition().x, 10, m_Camera01.getPosition().z);
	m_world = m_world * miniCamPosition;
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_Camera02.getCameraMatrix(), &m_projection, &m_Light, m_texture1.Get());
	m_ModeMinicam.Render(context);

	// render sphere
	m_world = SimpleMath::Matrix::Identity;
	m_world = m_world * SimpleMath::Matrix::CreateTranslation(0, 3, 0);
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_Camera02.getCameraMatrix(), &m_projection, &m_Light, m_texture1.Get());
	m_ModelSphere.Render(context);

	// render statue
	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix newPosition1 = SimpleMath::Matrix::CreateTranslation(5.0f, -0.7f, 0.0f);
	SimpleMath::Matrix newScale1 = SimpleMath::Matrix::CreateScale(1.5f, 1.5f, 1.5f);
	SimpleMath::Quaternion rot1 = SimpleMath::Quaternion::CreateFromYawPitchRoll(a2r * -90, 0, 0);
	m_world = m_world * SimpleMath::Matrix::CreateFromQuaternion(rot1) * newScale1 * newPosition1;
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_Camera02.getCameraMatrix(), &m_projection, &m_Light, m_texture2.Get());
	m_ModelStatue.Render(context);

	// render floor 
	m_world = SimpleMath::Matrix::Identity; //set world back to identity
	SimpleMath::Matrix newPosition3 = SimpleMath::Matrix::CreateTranslation(0.0f, -0.6f, 0.0f);
	m_world = m_world * newPosition3;
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_Camera02.getCameraMatrix(), &m_projection, &m_Light, m_texture1.Get());
	m_ModelFloor.Render(context);

	//prepare for stone.
	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix newScale4 = SimpleMath::Matrix::CreateScale(0.5f, 0.5f, 0.5f);
	SimpleMath::Matrix newPosition4 = SimpleMath::Matrix::CreateTranslation(-5, -0.7f, 0);
	SimpleMath::Quaternion rot4 = SimpleMath::Quaternion::CreateFromYawPitchRoll(a2r * 90, 0, 0);
	m_world = m_world * Matrix::CreateFromQuaternion(rot4) * newScale4 * newPosition4;
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_Camera02.getCameraMatrix(), &m_projection, &m_Light, m_texture3.Get());
	m_ModelStone.Render(context);

	//prepare for EvilDrone
	m_world = SimpleMath::Matrix::Identity;
	SimpleMath::Matrix newPosition5 = SimpleMath::Matrix::CreateTranslation(0, 3, 7);
	SimpleMath::Quaternion oldRotation5 = SimpleMath::Quaternion::CreateFromYawPitchRoll(a2r * 30, a2r * 40, a2r * 20);
	SimpleMath::Quaternion targetRotation5 = SimpleMath::Quaternion::CreateFromYawPitchRoll(a2r * 90, a2r * 90, a2r * 80);

	SimpleMath::Quaternion newRotation5 = SimpleMath::Quaternion::Slerp(oldRotation5, targetRotation5, lerpTimer);
	m_world = m_world * SimpleMath::Matrix::CreateTranslation(0, 7, 0) * SimpleMath::Matrix::CreateFromQuaternion(newRotation5) * newPosition5;
	m_BasicShaderPair.EnableShader(context);
	m_BasicShaderPair.SetShaderParameters(context, &m_world, &m_Camera02.getCameraMatrix(), &m_projection, &m_Light, m_texture4.Get());
	m_ModelEvilDrone.Render(context);



	// Reset the render target back to the original back buffer and not the render to texture anymore.	
	context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}

#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
	m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
	m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
	m_audEngine->Resume();
#endif
}

void Game::OnWindowMoved()
{
	auto r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateWindowSizeDependentResources();
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
	if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
	{
		// Setup a retry in 1 second
		m_audioTimerAcc = 1.f;
		m_retryDefault = true;
	}
}
#endif

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
	width = 800;
	height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto device = m_deviceResources->GetD3DDevice();

	m_states = std::make_unique<CommonStates>(device);
	m_fxFactory = std::make_unique<EffectFactory>(device);
	m_sprites = std::make_unique<SpriteBatch>(context);
	m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");
	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	//setup our test model
	m_ModelSphere.InitializeSphere(device, 1);
	m_ModelStatue.Initialize(device, "Statue.obj");
	m_ModelFloor.InitializeBox(device, 15, 0.1f, 15);
	m_ModeMinicam.InitializeSphere(device, 2);
	m_ModelStone.Initialize(device, "Stone.obj");
	m_ModelEvilDrone.Initialize(device, "drone.obj");

	//load and set up our Vertex and Pixel Shaders
	m_BasicShaderPair.InitStandard(device, L"light_vs.cso", L"light_ps.cso");

	//load Textures
	CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr, m_texture1.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Statue.dds", nullptr, m_texture2.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"Stone.dds", nullptr, m_texture3.ReleaseAndGetAddressOf());
	CreateDDSTextureFromFile(device, L"EvilDrone_Diff.dds", nullptr, m_texture4.ReleaseAndGetAddressOf());


	//Initialise Render to texture
	m_FirstRenderPass = new RenderTexture(device, 800, 600, 1, 2);	//for our rendering, We dont use the last two properties. but.  they cant be zero and they cant be the same. 

	m_SkyBox = new SkyBox(device, 10, 10);
	m_SkyBoxShaderPair.InitStandard(device, L"skymap_vs.cso", L"skymap_ps.cso");
	CreateDDSTextureFromFile(device, L"skybox.dds", nullptr, m_skyboxTexture.ReleaseAndGetAddressOf());

	m_ParticleSystem.Initialize(device);
	CreateDDSTextureFromFile(device, L"skybox.dds", nullptr, m_particlesystemTexture.ReleaseAndGetAddressOf());
	m_ParticleSystemShaderPair.InitStandard(device, L"particlesystem_vs.cso", L"particlesystem_ps.cso");


}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
	auto size = m_deviceResources->GetOutputSize();
	float aspectRatio = float(size.right) / float(size.bottom);
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	m_projection = Matrix::CreatePerspectiveFieldOfView(
		fovAngleY,	aspectRatio,
		0.01f,		100.0f
	);
}


void Game::OnDeviceLost()
{
	m_states.reset();
	m_fxFactory.reset();
	m_sprites.reset();
	m_font.reset();
	m_batch.reset();
	m_testmodel.reset();
	m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
	CreateDeviceDependentResources();

	CreateWindowSizeDependentResources();
}
#pragma endregion

#pragma region PrivateMethod

void Game::MainCameraControlUpdate(DX::StepTimer const& timer) {
	//note that currently.  Delta-time is not considered in the game object movement. 
	if (m_gameInputCommands.left)
	{
		Vector3 position = m_Camera01.getPosition(); //get the position
		position -= (m_Camera01.getRight() * m_Camera01.getMoveSpeed() * timer.GetElapsedSeconds()); //add the forward vector
		m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.right)
	{
		Vector3 position = m_Camera01.getPosition(); //get the position
		position += (m_Camera01.getRight() * m_Camera01.getMoveSpeed() * timer.GetElapsedSeconds()); //add the forward vector
		m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.forward)
	{
		Vector3 position = m_Camera01.getPosition(); //get the position
		position += (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * timer.GetElapsedSeconds()); //add the forward vector
		m_Camera01.setPosition(position);
	}
	if (m_gameInputCommands.back)
	{
		Vector3 position = m_Camera01.getPosition(); //get the position
		position -= (m_Camera01.getForward() * m_Camera01.getMoveSpeed() * timer.GetElapsedSeconds()); //add the forward vector
		m_Camera01.setPosition(position);
	}

	if (m_gameInputCommands.rotLeft || m_gameInputCommands.rotRight)
	{
		Vector3 rotation = m_Camera01.getRotation();
		rotation.y = rotation.y -= m_Camera01.getRotationSpeed() * timer.GetElapsedSeconds() * m_input.GetMouseDetX();
		m_Camera01.setRotation(rotation);
	}
	if (m_gameInputCommands.rotUp || m_gameInputCommands.rotDown)
	{
		Vector3 rotation = m_Camera01.getRotation();
		rotation.x = rotation.x -= m_Camera01.getRotationSpeed() * timer.GetElapsedSeconds() * m_input.GetMouseDetY();
		m_Camera01.setRotation(rotation);
	}

	m_Camera01.Update();	//camera update.

	m_view = m_Camera01.getCameraMatrix();
}

#pragma endregion