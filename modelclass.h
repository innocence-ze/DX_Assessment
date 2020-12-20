////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "pch.h"
//#include <d3dx10math.h>
//#include <fstream>
//using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////

using namespace DirectX;

class ModelClass
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};

	//struct OBJMaterial 
	//{
	//	XMFLOAT3 AmbientMaterial;
	//	XMFLOAT3 DiffuseMaterial;
	//	XMFLOAT3 SpecularMaterial;
	//	float SpecularPower;
	//};

public:
	ModelClass();
	~ModelClass();

	bool InitializeModel(ID3D11Device *device, char* filename);
	bool InitializeTeapot(ID3D11Device*);
	bool InitializeSphere(ID3D11Device*);
	bool InitializeBox(ID3D11Device*, float xwidth, float yheight, float zdepth);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	
	int GetIndexCount();

	////return the reference of data model's array
	//std::vector<VertexType*>& GetVertexArrayRef() { return m_vertexData; }

	////////////////////////////////////////////////////////////////////////////////////
	////set function
	////////////////////////////////////////////////////////////////////////////////////
	//void SetModeldName(std::string modelName)			{ m_objModelName = modelName; }
	//void SetMaterialName(std::string materialName)		{ m_objMaterialName = materialName; }
	//void SetAmbientMaterial(float r, float g, float b)	{ m_material->AmbientMaterial = XMFLOAT3(r, g, b); }
	//void SetDiffuseMaterial(float r, float g, float b)	{ m_material->DiffuseMaterial = XMFLOAT3(r, g, b); }
	//void SetSpecularMaterial(float r, float g, float b)	{ m_material->SpecularMaterial = XMFLOAT3(r, g, b); }
	//void SetSpecularPower(float specularPow)			{ m_material->SpecularPower = specularPow;}

	////////////////////////////////////////////////////////////////////////////////////
	////get function
	////////////////////////////////////////////////////////////////////////////////////
	//std::string GetMaterialName()						{ return m_objMaterialName;}
	//XMVECTOR GetAmbientMaterial()						{ return XMLoadFloat3(&m_material->AmbientMaterial); }
	//XMVECTOR GetDiffuseMaterial()						{ return XMLoadFloat3(&m_material->DiffuseMaterial); }
	//XMVECTOR GetSpecularMaterial()						{ return XMLoadFloat3(&m_material->SpecularMaterial); }
	//float GetSpecularPower()							{ return m_material->SpecularPower;}


private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadModel(char*);

	void ReleaseModel();
	//void ReleaseMaterial();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	////model data load from outside
	//std::vector<VertexType*> m_vertexData;
	//std::string m_objModelName, m_objMaterialName;
	//OBJMaterial* m_material;

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;

};

#endif