////////////////////////////////////////////////////////////////////////////////
// Filename: modeldataclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELDATACLASS_H_
#define _MODELDATACLASS_H_


//////////////
// INCLUDES //
//////////////
#include "pch.h"
//#include <d3dx10math.h>
//#include <fstream>
//using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelDataClass
////////////////////////////////////////////////////////////////////////////////

using namespace DirectX;

class ModelDataClass
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};

	struct OBJMaterial
	{
		XMFLOAT3 AmbientMaterial;
		XMFLOAT3 DiffuseMaterial;
		XMFLOAT3 SpecularMaterial;
		float SpecularPower;

		OBJMaterial()
		{
			AmbientMaterial = XMFLOAT3(1, 1, 1);
			DiffuseMaterial = XMFLOAT3(1, 1, 1);
			SpecularMaterial = XMFLOAT3(1, 1, 1);
			SpecularPower = 1;
		}
	};

public:
	ModelDataClass();
	~ModelDataClass();

	friend class ModelLoadClass;

	//create data, release data and render data
	bool InitializeModel(ID3D11Device* device);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount() { return m_indexCount; }

	//return the reference of data model's array
	std::vector<VertexType*>& GetVertexArrayRef() { return m_vertexData; }
	std::vector<uint16_t>& GetIndexArrayRef() { return m_IndexData; }

	//////////////////////////////////////////////////////////////////////////////////
	//set function of material
	//////////////////////////////////////////////////////////////////////////////////
	void SetModelName(std::string modelName) { m_objModelName = modelName; }
	void SetMaterialName(std::string materialName) { m_objMaterialName = materialName; }
	void SetAmbientMaterial(float r, float g, float b) { m_material->AmbientMaterial = XMFLOAT3(r, g, b); }
	void SetDiffuseMaterial(float r, float g, float b) { m_material->DiffuseMaterial = XMFLOAT3(r, g, b); }
	void SetSpecularMaterial(float r, float g, float b) { m_material->SpecularMaterial = XMFLOAT3(r, g, b); }
	void SetSpecularPower(float specularPow) { m_material->SpecularPower = specularPow; }

	//////////////////////////////////////////////////////////////////////////////////
	//get function of material
	//////////////////////////////////////////////////////////////////////////////////
	std::string GetMaterialName() { return m_objMaterialName; }
	XMVECTOR GetAmbientMaterial() { return XMLoadFloat3(&m_material->AmbientMaterial); }
	XMVECTOR GetDiffuseMaterial() { return XMLoadFloat3(&m_material->DiffuseMaterial); }
	XMVECTOR GetSpecularMaterial() { return XMLoadFloat3(&m_material->SpecularMaterial); }
	float GetSpecularPower() { return m_material->SpecularPower; }


private:
	// load various buffer
	bool InitializeBuffers(ID3D11Device*);
	// release various buffer
	void ShutdownBuffers();
	// render various buffer
	void RenderBuffers(ID3D11DeviceContext*);

	// release model data
	void ReleaseModel();
	// release material data
	void ReleaseMaterial();

	void ConvertToVertexType(const std::vector<DirectX::VertexPositionNormalTexture> vpnts);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;

	//model data load from outside
	std::vector<VertexType*> m_vertexData;
	std::vector<uint16_t> m_IndexData;
	std::string m_objModelName, m_objMaterialName;
	OBJMaterial* m_material;

};

#endif