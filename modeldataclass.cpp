////////////////////////////////////////////////////////////////////////////////
// Filename: modeldataclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "modelDataclass.h"

using namespace DirectX;

ModelDataClass::ModelDataClass()
{
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;

	m_vertexCount = 0;
	m_indexCount = 0;
	m_material = NULL;
}
ModelDataClass::~ModelDataClass()
{
}

bool ModelDataClass::InitializeModel(ID3D11Device* d3dDevice)
{
	bool result;
	m_material = new OBJMaterial();
	if (!m_material)
	{
		MessageBox(NULL, L"Initialize ObjMaterial failure", L"ERROR", MB_OK);
		return false;
	}

	result = InitializeBuffers(d3dDevice);
	if (!result) 
	{
		MessageBox(NULL, L"Initialize Buffer failure", L"ERROR", MB_OK);
		return false;
	}

	return true;
}

void ModelDataClass::Shutdown() 
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model and material data.
	ReleaseModel();
	ReleaseMaterial();
}

void ModelDataClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline
	RenderBuffers(deviceContext);
	deviceContext->DrawIndexed(m_indexCount, 0, 0);
}

bool ModelDataClass::InitializeBuffers(ID3D11Device* d3dDevice) 
{
	HRESULT result;

	VertexType* vertices;
	unsigned long* indices;

	m_vertexCount = m_vertexData.size();
	m_indexCount = m_IndexData.size();

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)	return false;

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)	return false;

	// Load the vertex array and index array
	for (size_t i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position	 = DirectX::SimpleMath::Vector3(m_vertexData[i]->position.x, m_vertexData[i]->position.y, m_vertexData[i]->position.z);
		vertices[i].texture		 = DirectX::SimpleMath::Vector2(m_vertexData[i]->texture.x, m_vertexData[i]->texture.y);
		vertices[i].normal		 = DirectX::SimpleMath::Vector3(m_vertexData[i]->normal.x, m_vertexData[i]->normal.y, m_vertexData[i]->normal.z);
	}
	for (int i = 0; i < m_indexCount; i++)
	{
		indices[i] = m_IndexData[i];
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth			= sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags		= 0;
	vertexBufferDesc.MiscFlags			= 0;
	vertexBufferDesc.StructureByteStride= 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem					= vertices;
	vertexData.SysMemPitch				= 0;
	vertexData.SysMemSlicePitch			= 0;
	result =  d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))					return false;


	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC  indexBufferDesc;
	indexBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth			= sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags		= 0;
	indexBufferDesc.MiscFlags			= 0;
	indexBufferDesc.StructureByteStride	= 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem					= indices;
	indexData.SysMemPitch				= 0;
	indexData.SysMemSlicePitch			= 0;
	result = d3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))					return false;

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void ModelDataClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelDataClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ModelDataClass::ReleaseModel()
{
	for (auto it = m_vertexData.begin(); it != m_vertexData.end(); ++it)
	{
		delete (*it);
		(*it) = NULL;
	}
}

void ModelDataClass::ReleaseMaterial()
{
	if (m_material) 
	{
		delete m_material;
		m_material = NULL;
	}
}

void ModelDataClass::ConvertToVertexType(const std::vector<DirectX::VertexPositionNormalTexture> vpnts)
{
	for (const auto& val : vpnts) 
	{
		ModelDataClass::VertexType* vertex = new ModelDataClass::VertexType();
		vertex->position = val.position;
		vertex->normal = val.normal;
		vertex->texture = val.textureCoordinate;
		m_vertexData.push_back(vertex);
	}
}
