/**************************
 file name: SkyBox.h

*/

#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#pragma once
#include <directxmath.h>

using namespace DirectX;

struct SkyBoxVertex
{
	SkyBoxVertex() {}
	SkyBoxVertex(float x, float y, float z,
				float u, float v,
				float nx, float ny, float nz)
				: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

class SkyBox
{
	public:
		SkyBox(ID3D11Device* device, int latitude, int longitude);
		~SkyBox();
		SkyBox();
		

		void Shutdown();
		void RenderBuffers(ID3D11DeviceContext* deviceContext);
		void Render(ID3D11DeviceContext* deviceContext);
		//void CreateSkyMap();

		//ID3D11Buffer*	GetIndexBuffer()	{ return m_sphereIndexBuffer; }
		//ID3D11Buffer*	GetVertexBuffer()	{ return m_sphereVertexBuffer; }
		//Shader*			GetShader()			{ return m_skyBoxShaderPair; }

	private:
		ID3D11Buffer*	m_sphereIndexBuffer;
		ID3D11Buffer*	m_sphereVertexBuffer;

		//ID3D11DepthStencilState* m_DSLessEqual;
		//ID3D11RasterizerState* m_RSCullNone;

		//Shader*			m_skyBoxShaderPair;
		int				m_numberSphereVertices;
		int				m_numberSphereFaces;
		XMMATRIX		m_rotationX;
		XMMATRIX		m_rotationY;
		XMMATRIX		m_rotationZ;

};

#endif 
