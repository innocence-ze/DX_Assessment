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

	//Initialize是创建元素,Shutdown是Release
	bool Initialize(ID3D11Device* d3dDevice, std::string OBJFileName);
	void Shutdown();

	//返回对象数组的拷贝
	std::vector<ModelDataClass*> GetObjModelArrayCopy() { return m_ObjModelArray; }


private:

	bool InitializeObjModelArray(ID3D11Device* d3dDevice);

	//释放OBJ对象
	void ReleaseObjModelArray();

	//加载OBJ文件
	bool LoadObjFile(std::string OBJFileName);

	//加载OBJ材质文件
	bool LoadObjMaterialFile(std::string MaterialFileName);


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

	std::vector<VertexPosition>		m_modelPosArray; //在外部加载的模型顶点位置
	std::vector<VertexNormal>		m_modelNormalArray; //在外部加载的模型数据
	std::vector<VertexTexture>		m_modelTextureArray; //在外部加载的模型数据纹理坐标
	std::vector<ModelDataClass*>	m_ObjModelArray; //OBJ对象数组
	std::string						m_ObjMaterialFileName;

};
#endif