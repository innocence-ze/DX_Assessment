////////////////////////////////////////////////////////////////////////////////
// Filename: modelloadclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELLOADCLASS_H_
#define _MODELLOADCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "modeldataclass.h"
//#include <d3dx10math.h>
//#include <fstream>
//using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelLoadClass
////////////////////////////////////////////////////////////////////////////////

using namespace DirectX;

class ModelLoadClass
{
public:
	ModelLoadClass();
	~ModelLoadClass();

	friend class ModelDataClass;

	//Initialize for creat, and Shutdown for Release
	bool Initialize(ID3D11Device* device, std::string OBJFileName);
	bool InitializeSphere(ID3D11Device* device, float radius);
	bool InitializeBox(ID3D11Device* device, float x, float y, float z);
	bool InitializeTeapot(ID3D11Device* device);
	void Shutdown();

	//return the array of model data
	std::vector<ModelDataClass*> GetModelArrayCopy() { return m_modelArray; }


private:

	bool InitializeModelArray(ID3D11Device* d3dDevice);

	//释放OBJ对象
	void ReleaseModelArray();

	//加载OBJ文件
	bool LoadModelFile(std::string OBJFileName);

	//加载OBJ材质文件
	bool LoadMaterialFile(std::string MaterialFileName);


private:
	struct VertexPosition
	{
		float x, y, z;
		VertexPosition(float a, float b, float c) :x(a), y(b), z(c){}
	};
	struct VertexNormal
	{
		float nx, ny, nz;
		VertexNormal(float a, float b, float c) : nx(a), ny(b), nz(c){}
	};
	struct VertexTexture
	{
		float u, v;
		VertexTexture(float a, float b) : u(a), v(b){}
	};

	std::vector<VertexPosition>		m_modelPosArray;		// vertex position in externally loaded model
	std::vector<VertexNormal>		m_modelNormalArray;		// vertex normal in externally loaded model
	std::vector<VertexTexture>		m_modelTextureArray;	// vertex texture in externally loaded model
	std::vector<ModelDataClass*>	m_modelArray;			// array of model data
	std::string						m_materialFileName;		// name of material file, and "" represents without material

};
#endif