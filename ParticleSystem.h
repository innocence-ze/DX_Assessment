/**************************
 file name: ParticleSystem.h

*/



#pragma once
#include <directxmath.h>

using namespace DirectX;

class ParticleSystem
{
private:
	struct ParticleType
	{
		float positionX, positionY, positionZ;
		float red, green, blue;
		float velocity;
		bool active;
	};
	struct ParticleVertexType
	{
		SimpleMath::Vector3 position;
		SimpleMath::Vector2 texture;
		SimpleMath::Vector4 color;
	};


public:
	ParticleSystem();
	ParticleSystem(const ParticleSystem&);
	~ParticleSystem();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	bool Frame(float, ID3D11DeviceContext*);
	void Render(ID3D11DeviceContext*);
	
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture()	{ return m_Texture; }
	int GetIndexCount()												{ return m_indexCount; }

private:
	//bool LoadTexture(ID3D11Device*, WCHAR*);

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();

	bool UpdateBuffers(ID3D11DeviceContext*);

	void RenderBuffers(ID3D11DeviceContext*);


	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;

	int m_currentParticleCount;
	float m_accumulatedTime;

	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Texture;
	ParticleType* m_particleList;
	int m_vertexCount, m_indexCount;
	ParticleVertexType* m_vertices;
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;

};