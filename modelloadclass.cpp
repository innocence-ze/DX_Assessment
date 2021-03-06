////////////////////////////////////////////////////////////////////////////////
// Filename: modelloadclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "modelloadclass.h"

#include <sstream>

using namespace DirectX;

ModelLoadClass::ModelLoadClass()
{

}

ModelLoadClass::~ModelLoadClass()
{

}

void ModelLoadClass::Render(ID3D11DeviceContext* context)
{
	for (size_t i = 0; i < m_modelArray.size(); i++)
	{
		m_modelArray[i]->Render(context);
	}
}

void ModelLoadClass::Shutdown()
{
	ReleaseModelArray();
}

void ModelLoadClass::ReleaseModelArray()
{
	for (size_t i = 0; i < m_modelArray.size(); ++i)
	{
		m_modelArray[i]->Shutdown();
		delete m_modelArray[i];
		m_modelArray[i] = NULL;
	}
}

bool ModelLoadClass::Initialize(ID3D11Device* device, std::string objFileName)
{
	bool result;

	m_materialFileName = "";

	//load obj file, put in the vector of ModelDataClass
	result = LoadModelFile(objFileName);
	if (!result)
	{
		MessageBox(NULL, L"Load .obj file failure", NULL, MB_OK);
		return false;
	}

	//initialize the object in the vector of ModelDataClass
	result = InitializeModelArray(device);
	if (!result)
	{
		MessageBox(NULL, L"OBJModelArray failure", NULL, MB_OK);
		return false;
	}


	if (m_materialFileName == "")
	{
		return true;
	}

	//load material file of obj
	result = LoadMaterialFile(m_materialFileName);
	if (!result)
	{
		MessageBox(NULL, L"Load OBJ Material file failure", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ModelLoadClass::InitializeSphere(ID3D11Device* device, float radius)
{
	ModelDataClass* model = new ModelDataClass();

	std::vector<DirectX::VertexPositionNormalTexture> vpnts;
	GeometricPrimitive::CreateSphere(vpnts, model->GetIndexArrayRef(), radius, 8, false);
	model->ConvertToVertexType(vpnts);

	m_modelArray.push_back(model);

	bool result = InitializeModelArray(device);
	return result;
}

bool ModelLoadClass::InitializeBox(ID3D11Device* device, float x, float y, float z)
{
	ModelDataClass* model = new ModelDataClass();

	std::vector<DirectX::VertexPositionNormalTexture> vpnts;
	GeometricPrimitive::CreateBox(vpnts, model->GetIndexArrayRef(), DirectX::SimpleMath::Vector3(x, y, z), false);
	model->ConvertToVertexType(vpnts);

	m_modelArray.push_back(model);

	bool result = InitializeModelArray(device);
	return result;
}

bool ModelLoadClass::InitializeTeapot(ID3D11Device* device, float size)
{
	ModelDataClass* model = new ModelDataClass();

	std::vector<DirectX::VertexPositionNormalTexture> vpnts;
	GeometricPrimitive::CreateTeapot(vpnts, model->GetIndexArrayRef(), size, 8, false);
	model->ConvertToVertexType(vpnts);

	m_modelArray.push_back(model);

	bool result = InitializeModelArray(device);
	return result;
}

bool ModelLoadClass::InitializeModelArray(ID3D11Device* device)
{
	bool result;
	for (size_t i = 0; i < m_modelArray.size(); ++i)
	{
		result = m_modelArray[i]->InitializeModel(device);
		if (!result)		return false;
	}
	return true;
}

bool ModelLoadClass::LoadModelFile(std::string ObjFileName)
{
	std::ifstream in(ObjFileName);
	if (!in)		return false;

	// line represent the whole line in the file, include space
	// word represent the only one word in the file, exclude space
	std::string line, word;

	bool lastLineFace = false;
	ModelDataClass* memObjModel;

	int tempIndex = 0;

	while (!in.eof())
	{

		getline(in, line);
		std::istringstream record(line);
		record >> word;
		if (word.find("#") != std::string::npos)
		{

		}
		//if line contains mtllib, store the path of obj's material
		else if (word.find("mtllib") != std::string::npos)
		{
			record >> word;
			m_materialFileName = word;
		}
		else if (word == "v")
		{
			record >> word;
			float PosX = atof(&word[0]);
			record >> word;
			float PosY = atof(&word[0]);
			record >> word;
			float PosZ = atof(&word[0]);
			m_modelPosArray.push_back(VertexPosition(PosX, PosY, PosZ));
		}
		else if (word == "vt")
		{
			record >> word;
			float u = atof(&word[0]);
			record >> word;
			float v = atof(&word[0]);
			m_modelTextureArray.push_back(VertexTexture(u, v));
		}
		else if (word == "vn")
		{
			record >> word;
			float NormalX = atof(&word[0]);
			record >> word;
			float NormalY = atof(&word[0]);
			record >> word;
			float NormalZ = atof(&word[0]);
			m_modelNormalArray.push_back(VertexNormal(NormalX, NormalY, NormalZ));
		}
		else if (word == "o" || word == "g")
		{
			memObjModel = new ModelDataClass();
			record >> word;
			memObjModel->SetModelName(word);
		}
		else if (word == "usemtl")
		{
			record >> word;
			memObjModel->SetMaterialName(word);

		}
		else if (word == "f")
		{
			lastLineFace = true;
			ModelDataClass::VertexType* vertex1 = new ModelDataClass::VertexType();
			if (!vertex1)		return false;

			ModelDataClass::VertexType* vertex2 = new ModelDataClass::VertexType();
			if (!vertex2)		return false;

			ModelDataClass::VertexType* vertex3 = new ModelDataClass::VertexType();
			if (!vertex3)		return false;

			//whether have texture coordinate, some dont have
			bool IsTexExist;
			record >> word;

			//whether have texcoordinate
			if (word.find("//") == std::string::npos)		IsTexExist = true;
			else											IsTexExist = false;

			//if dont contains ��//��, read vertex, texture, normal
			if (IsTexExist)
			{
				///////////////////////////
				///////first vertex////////
				///////////////////////////
				/*dissect vertex position*/
				//find the position of first char "/" in the word
				UINT FirstIndex = word.find("/");
				std::string NumericStr = word.substr(0, FirstIndex);
				UINT PosIndex = atoi(&NumericStr[0]);
				vertex1->position.x = m_modelPosArray[PosIndex - 1].x;
				vertex1->position.y = m_modelPosArray[PosIndex - 1].y;
				vertex1->position.z = m_modelPosArray[PosIndex - 1].z;

				/*dissect vertex texture*/
				//find the position of second char "/" in the word
				UINT SecondIndex = word.find("/", FirstIndex + 1);
				NumericStr = word.substr(FirstIndex + 1, SecondIndex - FirstIndex - 1);
				PosIndex = atoi(&NumericStr[0]);
				vertex1->texture.x = m_modelTextureArray[PosIndex - 1].u;
				vertex1->texture.y = m_modelTextureArray[PosIndex - 1].v;

				/*dissect vertex normal*/
				NumericStr = word.substr(SecondIndex + 1);
				PosIndex = atoi(&NumericStr[0]);
				vertex1->normal.x = m_modelNormalArray[PosIndex - 1].nx;
				vertex1->normal.y = m_modelNormalArray[PosIndex - 1].ny;
				vertex1->normal.z = m_modelNormalArray[PosIndex - 1].nz;


				///////////////////////////
				///////second vertex///////
				///////////////////////////
				record >> word;
				FirstIndex = word.find("/");
				NumericStr = word.substr(0, FirstIndex);
				PosIndex = atoi(&NumericStr[0]);
				vertex2->position.x = m_modelPosArray[PosIndex - 1].x;
				vertex2->position.y = m_modelPosArray[PosIndex - 1].y;
				vertex2->position.z = m_modelPosArray[PosIndex - 1].z;


				SecondIndex = word.find("/", FirstIndex + 1);
				NumericStr = word.substr(FirstIndex + 1, SecondIndex - FirstIndex - 1);
				PosIndex = atoi(&NumericStr[0]);
				vertex2->texture.x = m_modelTextureArray[PosIndex - 1].u;
				vertex2->texture.y = m_modelTextureArray[PosIndex - 1].v;


				NumericStr = word.substr(SecondIndex + 1);
				PosIndex = atoi(&NumericStr[0]);
				vertex2->normal.x = m_modelNormalArray[PosIndex - 1].nx;
				vertex2->normal.y = m_modelNormalArray[PosIndex - 1].ny;
				vertex2->normal.z = m_modelNormalArray[PosIndex - 1].nz;


				///////////////////////////
				///////third vertex////////
				///////////////////////////
				record >> word;
				FirstIndex = word.find("/");
				NumericStr = word.substr(0, FirstIndex);
				PosIndex = atoi(&NumericStr[0]);
				vertex3->position.x = m_modelPosArray[PosIndex - 1].x;
				vertex3->position.y = m_modelPosArray[PosIndex - 1].y;
				vertex3->position.z = m_modelPosArray[PosIndex - 1].z;


				SecondIndex = word.find("/", FirstIndex + 1);
				NumericStr = word.substr(FirstIndex + 1, SecondIndex - FirstIndex - 1);
				PosIndex = atoi(&NumericStr[0]);
				vertex3->texture.x = m_modelTextureArray[PosIndex - 1].u;
				vertex3->texture.y = m_modelTextureArray[PosIndex - 1].v;


				NumericStr = word.substr(SecondIndex + 1);
				PosIndex = atoi(&NumericStr[0]);
				vertex3->normal.x = m_modelNormalArray[PosIndex - 1].nx;
				vertex3->normal.y = m_modelNormalArray[PosIndex - 1].ny;
				vertex3->normal.z = m_modelNormalArray[PosIndex - 1].nz;

			}
			//if contains "//"
			else
			{

				///////////////////////////
				///////first vertex////////
				///////////////////////////
				UINT FirstIndex = word.find("//");
				std::string NumericStr = word.substr(0, FirstIndex);
				UINT PosIndex = atoi(&NumericStr[0]);
				vertex1->position.x = m_modelPosArray[PosIndex - 1].x;
				vertex1->position.y = m_modelPosArray[PosIndex - 1].y;
				vertex1->position.z = m_modelPosArray[PosIndex - 1].z;


				NumericStr = word.substr(FirstIndex + 2);
				PosIndex = atoi(&NumericStr[0]);
				vertex1->normal.x = m_modelNormalArray[PosIndex - 1].nx;
				vertex1->normal.y = m_modelNormalArray[PosIndex - 1].ny;
				vertex1->normal.z = m_modelNormalArray[PosIndex - 1].nz;
				//assign uv casually
				vertex1->texture.x = 0.0f;
				vertex1->texture.y = 0.0f;



				///////////////////////////
				///////second vertex///////
				///////////////////////////
				record >> word;
				FirstIndex = word.find("//");
				NumericStr = word.substr(0, FirstIndex);
				PosIndex = atoi(&NumericStr[0]);
				vertex2->position.x = m_modelPosArray[PosIndex - 1].x;
				vertex2->position.y = m_modelPosArray[PosIndex - 1].y;
				vertex2->position.z = m_modelPosArray[PosIndex - 1].z;


				NumericStr = word.substr(FirstIndex + 2);
				PosIndex = atoi(&NumericStr[0]);
				vertex2->normal.x = m_modelNormalArray[PosIndex - 1].nx;
				vertex2->normal.y = m_modelNormalArray[PosIndex - 1].ny;
				vertex2->normal.z = m_modelNormalArray[PosIndex - 1].nz;
				//assign uv casually
				vertex2->texture.x = 0.0f;
				vertex2->texture.y = 0.0f;



				///////////////////////////
				///////third vertex////////
				///////////////////////////
				record >> word;
				FirstIndex = word.find("//");
				NumericStr = word.substr(0, FirstIndex);
				PosIndex = atoi(&NumericStr[0]);
				vertex3->position.x = m_modelPosArray[PosIndex - 1].x;
				vertex3->position.y = m_modelPosArray[PosIndex - 1].y;
				vertex3->position.z = m_modelPosArray[PosIndex - 1].z;


				NumericStr = word.substr(FirstIndex + 2);
				PosIndex = atoi(&NumericStr[0]);
				vertex3->normal.x = m_modelNormalArray[PosIndex - 1].nx;
				vertex3->normal.y = m_modelNormalArray[PosIndex - 1].ny;
				vertex3->normal.z = m_modelNormalArray[PosIndex - 1].nz;
				//assign uv casually
				vertex3->texture.x = 0.0f;
				vertex3->texture.y = 0.0f;

			}

			memObjModel->GetVertexArrayRef().push_back(vertex1);
			memObjModel->GetVertexArrayRef().push_back(vertex2);
			memObjModel->GetVertexArrayRef().push_back(vertex3);
			memObjModel->GetIndexArrayRef().push_back(tempIndex);
			memObjModel->GetIndexArrayRef().push_back(tempIndex + 1);
			memObjModel->GetIndexArrayRef().push_back(tempIndex + 2);
			tempIndex += 3;

			//have four vertices in one face
			if (!record.eof())
			{
				// 4 vertices
				ModelDataClass::VertexType* vertex4 = new ModelDataClass::VertexType();
				if (!vertex4)		return false;

				record >> word;

				//if dont contains ��//��, read vertex, texture, normal
				if (IsTexExist)
				{
					///////////////////////////
					///////fourth vertex///////
					///////////////////////////
					/*dissect vertex position*/
					//find the position of first char "/" in the word
					UINT FirstIndex = word.find("/");
					std::string NumericStr = word.substr(0, FirstIndex);
					UINT PosIndex = atoi(&NumericStr[0]);
					vertex4->position.x = m_modelPosArray[PosIndex - 1].x;
					vertex4->position.y = m_modelPosArray[PosIndex - 1].y;
					vertex4->position.z = m_modelPosArray[PosIndex - 1].z;

					/*dissect vertex texture*/
					//find the position of second char "/" in the word
					UINT SecondIndex = word.find("/", FirstIndex + 1);
					NumericStr = word.substr(FirstIndex + 1, SecondIndex - FirstIndex - 1);
					PosIndex = atoi(&NumericStr[0]);
					vertex4->texture.x = m_modelTextureArray[PosIndex - 1].u;
					vertex4->texture.y = m_modelTextureArray[PosIndex - 1].v;

					/*dissect vertex normal*/
					NumericStr = word.substr(SecondIndex + 1);
					PosIndex = atoi(&NumericStr[0]);
					vertex4->normal.x = m_modelNormalArray[PosIndex - 1].nx;
					vertex4->normal.y = m_modelNormalArray[PosIndex - 1].ny;
					vertex4->normal.z = m_modelNormalArray[PosIndex - 1].nz;
				}

				//if contains "//"
				else
				{

					///////////////////////////
					///////fourth vertex///////
					///////////////////////////
					UINT FirstIndex = word.find("//");
					std::string NumericStr = word.substr(0, FirstIndex);
					UINT PosIndex = atoi(&NumericStr[0]);
					vertex4->position.x = m_modelPosArray[PosIndex - 1].x;
					vertex4->position.y = m_modelPosArray[PosIndex - 1].y;
					vertex4->position.z = m_modelPosArray[PosIndex - 1].z;


					NumericStr = word.substr(FirstIndex + 2);
					PosIndex = atoi(&NumericStr[0]);
					vertex4->normal.x = m_modelNormalArray[PosIndex - 1].nx;
					vertex4->normal.y = m_modelNormalArray[PosIndex - 1].ny;
					vertex4->normal.z = m_modelNormalArray[PosIndex - 1].nz;
					//assign uv casually
					vertex4->texture.x = 0.0f;
					vertex4->texture.y = 0.0f;

				}
				memObjModel->GetVertexArrayRef().push_back(vertex1);
				memObjModel->GetVertexArrayRef().push_back(vertex3);
				memObjModel->GetVertexArrayRef().push_back(vertex4);
				memObjModel->GetIndexArrayRef().push_back(tempIndex);
				memObjModel->GetIndexArrayRef().push_back(tempIndex + 1);
				memObjModel->GetIndexArrayRef().push_back(tempIndex + 2);
				tempIndex += 3;
			}
		}
		if (lastLineFace && line[0] != 'f')
		{
			lastLineFace = false;
			tempIndex = 0;
			m_modelArray.push_back(memObjModel);
		}

	}

	return true;
}

bool ModelLoadClass::LoadMaterialFile(std::string MaterialFileName)
{
	std::ifstream in(MaterialFileName);
	if (!in)
	{
		return false;
	}



	std::string line, word;
	std::vector<UINT> indices;

	//read number of vertex in the first line (three of them is a triangle)
	while (!in.eof())
	{
		getline(in, line);

		//if current line contains "newmtl", find its material
		if (line.find("newmtl") != std::string::npos)
		{
			indices.clear();
			std::istringstream record(line);
			record >> word;
			record >> word;

			//traversal whole array, find the model which have corresponding material
			for (size_t i = 0; i < m_modelArray.size(); ++i)
			{
				if (word == m_modelArray[i]->GetMaterialName())
				{
					indices.push_back(i);
				}
			}

			//enter inner circle�� read property of respond material
			while (!line.empty())
			{

				getline(in, line);

				//specular power
				if (line.find("Ns") != std::string::npos)
				{
					std::istringstream re(line);
					re >> word;
					re >> word;
					float SpecularPower = atof(&word[0]);
					for (auto index : indices)
					{
						m_modelArray[index]->SetSpecularPower(SpecularPower);
					}
				}
				//ambient color
				else if (line.find("Ka") != std::string::npos)
				{
					std::istringstream re(line);
					re >> word;
					re >> word;
					float r = atof(&word[0]);
					re >> word;
					float g = atof(&word[0]);
					re >> word;
					float b = atof(&word[0]);
					for (auto index : indices)
					{
						m_modelArray[index]->SetAmbientMaterial(r, g, b);
					}
				}
				//diffuse color
				else if (line.find("Kd") != std::string::npos)
				{
					std::istringstream re(line);
					re >> word;
					re >> word;
					float r = atof(&word[0]);
					re >> word;
					float g = atof(&word[0]);
					re >> word;
					float b = atof(&word[0]);
					for (auto index : indices)
					{
						m_modelArray[index]->SetDiffuseMaterial(r, g, b);
					}

				}
				//specular color
				else if (line.find("Ks") != std::string::npos)
				{
					std::istringstream re(line);
					re >> word;
					re >> word;
					float r = atof(&word[0]);
					re >> word;
					float g = atof(&word[0]);
					re >> word;
					float b = atof(&word[0]);
					for (auto index : indices)
					{
						m_modelArray[index]->SetSpecularMaterial(r, g, b);
					}
				}

			}
		}

	}

	return true;
}




