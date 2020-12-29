#include "pch.h"
#include "SkyBox.h"

SkyBox::SkyBox(ID3D11Device* device, int latitudeLines, int longitudeLines)
{
	m_numberSphereVertices = ((latitudeLines - 2) * longitudeLines) + 2;
	m_numberSphereFaces = ((latitudeLines - 3) * longitudeLines * 2) + (longitudeLines * 2);

	float sphereYaw = 0;
	float spherePitch = 0;

	std::vector<SkyBoxVertex> vertices(m_numberSphereVertices);

	XMVECTOR curVertexPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	//set pos for vertices
	vertices[0].pos.x = 0.0f;
	vertices[0].pos.y = 0.0f;
	vertices[0].pos.z = 1.0f;
	for (int i = 0; i < latitudeLines - 2; i++)
	{
		spherePitch = (i + 1) * (3.14f / (latitudeLines - 1.0f));
		m_rotationX = XMMatrixRotationX(spherePitch);
		for (int j = 0; j < longitudeLines; ++j)
		{
			sphereYaw = j * (6.28 / (longitudeLines));
			m_rotationY = XMMatrixRotationZ(sphereYaw);
			curVertexPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (m_rotationX * m_rotationY));
			curVertexPos = XMVector3Normalize(curVertexPos);
			vertices[i * longitudeLines + j + 1].pos.x = XMVectorGetX(curVertexPos);
			vertices[i * longitudeLines + j + 1].pos.y = XMVectorGetY(curVertexPos);
			vertices[i * longitudeLines + j + 1].pos.z = XMVectorGetZ(curVertexPos);
		}
	}
	vertices[m_numberSphereVertices - 1].pos.x = 0.0f;
	vertices[m_numberSphereVertices - 1].pos.y = 0.0f;
	vertices[m_numberSphereVertices - 1].pos.z = -1.0f;


	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SkyBoxVertex) * m_numberSphereVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &vertices[0];
	device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_sphereVertexBuffer);


	std::vector<int> indices(m_numberSphereFaces * 3);
	//set index
	int k = 0;

	//draw circle
	for (int l = 0; l < longitudeLines - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 2;
		indices[k + 2] = l + 1;
		k += 3;
	}
	indices[k] = 0;
	indices[k + 1] = 1;
	indices[k + 2] = longitudeLines;
	k += 3;

	//drwa quad
	for (DWORD i = 0; i < latitudeLines - 3; ++i)
	{
		for (DWORD j = 0; j < longitudeLines - 1; ++j)
		{
			indices[k] = i * longitudeLines + j + 1;
			indices[k + 1] = i * longitudeLines + j + 2;
			indices[k + 2] = (i + 1) * longitudeLines + j + 1;

			indices[k + 3] = (i + 1) * longitudeLines + j + 1;
			indices[k + 4] = i * longitudeLines + j + 2;
			indices[k + 5] = (i + 1) * longitudeLines + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i * longitudeLines) + longitudeLines;
		indices[k + 1] = (i * longitudeLines) + 1;
		indices[k + 2] = ((i + 1) * longitudeLines) + longitudeLines;

		indices[k + 3] = ((i + 1) * longitudeLines) + longitudeLines;
		indices[k + 4] = (i * longitudeLines) + 1;
		indices[k + 5] = ((i + 1) * longitudeLines) + 1;

		k += 6;
	}

	//draw circle
	for (DWORD l = 0; l < longitudeLines - 1; ++l)
	{
		indices[k] = m_numberSphereVertices - 1;
		indices[k + 1] = (m_numberSphereVertices - 1) - (l + 2);
		indices[k + 2] = (m_numberSphereVertices - 1) - (l + 1);
		k += 3;
	}
	indices[k] = m_numberSphereVertices - 1 - longitudeLines;
	indices[k + 1] = (m_numberSphereVertices - 1);
	indices[k + 2] = m_numberSphereVertices - 2;


	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * m_numberSphereFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = &indices[0];
	device->CreateBuffer(&indexBufferDesc, &iinitData, &m_sphereIndexBuffer);



	//D3D11_RASTERIZER_DESC cmdesc;
	//cmdesc.CullMode = D3D11_CULL_NONE;
	//device->CreateRasterizerState(&cmdesc, &m_RSCullNone);

	//D3D11_DEPTH_STENCIL_DESC dssDesc;
	//ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	//dssDesc.DepthEnable = true;
	//dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	//device->CreateDepthStencilState(&dssDesc, &m_DSLessEqual);
}

SkyBox::~SkyBox()
{

}

SkyBox::SkyBox()
{
	
}

void SkyBox::Shutdown()
{
	m_sphereIndexBuffer->Release();
	m_sphereVertexBuffer->Release();
}


void SkyBox::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(SkyBoxVertex);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_sphereVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void SkyBox::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline
	RenderBuffers(deviceContext);
	deviceContext->DrawIndexed(m_numberSphereFaces * 3, 0, 0);
}