#include "pch.h"
#include "Input.h"


Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
	m_quitApp = false;

	m_GameInput.forward		= false;
	m_GameInput.back		= false;
	m_GameInput.right		= false;
	m_GameInput.left		= false;

	m_GameInput.rotRight	= false;
	m_GameInput.rotLeft		= false;
	m_GameInput.rotUp		= false;
	m_GameInput.rotDown		= false;

	m_mouseLastX = m_mouse->GetState().x;
	m_mouseLastY = m_mouse->GetState().y;
	m_mouseDetX  = 0;
	m_mouseDetY  = 0;
}

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	//A key
	if (kb.A)	m_GameInput.left = true;
	else		m_GameInput.left = false;
	
	//D key
	if (kb.D)	m_GameInput.right = true;
	else		m_GameInput.right = false;

	//W key
	if (kb.W)	m_GameInput.forward	 = true;
	else		m_GameInput.forward = false;

	//S key
	if (kb.S)	m_GameInput.back = true;
	else		m_GameInput.back = false;

	if (m_KeyboardTracker.IsKeyPressed(DirectX::Keyboard::F))	m_GameInput.wireframe = (m_GameInput.wireframe ? false : true);
	if (m_KeyboardTracker.IsKeyPressed(DirectX::Keyboard::M))	m_GameInput.miniMap = (m_GameInput.miniMap ? false : true);
	if (m_KeyboardTracker.IsKeyPressed(DirectX::Keyboard::P))	m_GameInput.pauseMusic = (m_GameInput.pauseMusic ? false : true);

	m_mouseDetX = mouse.x - m_mouseLastX;
	m_mouseDetY = mouse.y - m_mouseLastY;
	m_mouseLastX = mouse.x;
	m_mouseLastY = mouse.y;

	//mouse right button pressed for rotate
	if (m_MouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::ButtonState::HELD) 
	{
		if (m_mouseDetX < 0)	m_GameInput.rotLeft = true;
		else					m_GameInput.rotLeft = false;

		if (m_mouseDetX > 0)	m_GameInput.rotRight = true;
		else					m_GameInput.rotRight = false;

		if (m_mouseDetY < 0)	m_GameInput.rotDown = true;
		else					m_GameInput.rotDown = false;

		if (m_mouseDetY > 0)	m_GameInput.rotUp = true;
		else					m_GameInput.rotUp = false;
	}

}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}

int Input::GetMouseDetX()
{
	return m_mouseDetX;
}

int Input::GetMouseDetY() 
{
	return m_mouseDetY;
}

bool Input::GetMouseButtonDown(int mouse)
{
	if (mouse == 0 && m_MouseTracker.leftButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED)
	{
		return true;
	}
	if (mouse == 1 && m_MouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED)
	{
		return true;
	}
	return false;
}