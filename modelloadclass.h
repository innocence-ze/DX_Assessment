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

	//Initialize�Ǵ���Ԫ��,Shutdown��Release
	bool Initialize(ID3D11Device* d3dDevice, std::string OBJFileName);
	void Shutdown();

	//���ض�������Ŀ���
	std::vector<ModelDataClass*> GetObjModelArrayCopy() { return m_ObjModelArray; }


private:

	bool InitializeObjModelArray(ID3D11Device* d3dDevice);

	//�ͷ�OBJ����
	void ReleaseObjModelArray();

	//����OBJ�ļ�
	bool LoadObjFile(std::string OBJFileName);

	//����OBJ�����ļ�
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

	std::vector<VertexPosition>		m_modelPosArray; //���ⲿ���ص�ģ�Ͷ���λ��
	std::vector<VertexNormal>		m_modelNormalArray; //���ⲿ���ص�ģ������
	std::vector<VertexTexture>		m_modelTextureArray; //���ⲿ���ص�ģ��������������
	std::vector<ModelDataClass*>	m_ObjModelArray; //OBJ��������
	std::string						m_ObjMaterialFileName;

};
#endif