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

void ModelLoadClass::Shutdown() 
{
	ReleaseObjModelArray();
}

void ModelLoadClass::ReleaseObjModelArray()
{
	for (size_t i = 0; i < m_ObjModelArray.size(); ++i)
	{
		m_ObjModelArray[i]->Shutdown();
		delete m_ObjModelArray[i];
		m_ObjModelArray[i] = NULL;
	}
}

bool ModelLoadClass::Initialize(ID3D11Device* device, std::string objFileName)
{
	bool result;

	//load obj file, put in the vector of ModelDataClass
	result = LoadObjFile(objFileName);
	if (!result)
	{
		MessageBox(NULL, L"Load .obj file failure", NULL, MB_OK);
		return false;
	}

	//initialize the object in the vector of ModelDataClass
	result = InitializeObjModelArray(device);
	if (!result)
	{
		MessageBox(NULL, L"OBJModelArray failure", NULL, MB_OK);
		return false;
	}

	//load material file of obj
	result = LoadObjMaterialFile(m_ObjMaterialFileName);
	if (!result)
	{
		MessageBox(NULL, L"Load OBJ Material file failure", NULL, MB_OK);
		return false;
	}

	return true;
}

bool ModelLoadClass::InitializeObjModelArray(ID3D11Device* device)
{
	bool result;
	for (size_t i = 0; i < m_ObjModelArray.size(); ++i)
	{
		result = m_ObjModelArray[i]->InitializeModel(device);
		if (!result)		return false;
	}
	return true;
}

bool ModelLoadClass::LoadObjFile(std::string ObjFileName)
{
	std::ifstream in(ObjFileName);
	if (!in)		return false;

	// line represent the whole line in the file, include space
	// word represent the only one word in the file, exclude space
	std::string line, word;

	unsigned int NullStrCount = 0;

	unsigned int objCount = 0;

	while (!in.eof())
	{

		getline(in, line);


		//if line contains mtllib, store the path of obj's material
		if (line.find("mtllib") != std::string::npos)
		{
			std::string MaterialFileName;
			std::istringstream record(line);
			record >> word;
			record >> word;
			MaterialFileName = word;
			m_ObjMaterialFileName = MaterialFileName;
		}
		else if (line.find("Object") != std::string::npos)
		{
			std::istringstream record(line);
			record >> word;
			record >> word;
			record >> word;
			ModelDataClass* memObjModel = new ModelDataClass();
			if (!memObjModel)		return false;
			
			std::vector<ModelDataClass::VertexType*>& mVertexArray = memObjModel->GetVertexArrayRef();;

			memObjModel->SetModelName(word);
			while (1)
			{
				getline(in, line);

				std::istringstream re(line);
				re >> word;

				//if line contains faces, end circle
				if (line.find("faces") != std::string::npos)
				{
					// store model object into Array before quit circle
					m_ObjModelArray.push_back(memObjModel);
					break;
				}

				//if line contains usemtl, store material name of each obj object in the material file
				if (line.find("usemtl") != std::string::npos)
				{
					std::istringstream re1(line);
					re1 >> word;
					re1 >> word;
					memObjModel->SetMaterialName(word);
				}

				//whether line begins with v, vn, vt, f (vertex, normal, texture, face)
				if (word == std::string("v"))
				{
					re >> word;
					float PosX = atof(&word[0]);
					re >> word;
					float PosY = atof(&word[0]);
					re >> word;
					float PosZ = atof(&word[0]);
					m_modelPosArray.push_back(VertexPosition(PosX, PosY, PosZ));

				}
				else if (word == std::string("vn"))
				{
					re >> word;
					float NormalX = atof(&word[0]);
					re >> word;
					float NormalY = atof(&word[0]);
					re >> word;
					float NormalZ = atof(&word[0]);
					m_modelNormalArray.push_back(VertexNormal(NormalX, NormalY, NormalZ));
				}
				else if (word == std::string("vt"))
				{
					re >> word;
					float u = atof(&word[0]);
					re >> word;
					float v = atof(&word[0]);
					m_modelTextureArray.push_back(VertexTexture(u, v));
				}
				else if (word == std::string("f"))
				{
					//1 face has 3 vertices
					ModelDataClass::VertexType* vertex1 = new ModelDataClass::VertexType();
					if (!vertex1)		return false;

					ModelDataClass::VertexType* vertex2 = new ModelDataClass::VertexType();
					if (!vertex2)		return false;

					ModelDataClass::VertexType* vertex3 = new ModelDataClass::VertexType();
					if (!vertex3)		return false;


					//whether have texture coordinate, some dont have
					bool IsTexExist;
					re >> word;


					if (word.find("//") == std::string::npos)		IsTexExist = true;
					else											IsTexExist = false;

					//if dont contains ¡°//¡±, read vertex, texture, normal
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
						re >> word;
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
						re >> word;
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
						re >> word;
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
						re >> word;
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

					mVertexArray.push_back(vertex1);
					mVertexArray.push_back(vertex2);
					mVertexArray.push_back(vertex3);
				}
			}
		}
	}

	return true;
}

bool ModelLoadClass::LoadObjMaterialFile(std::string MaterialFileName)
{
	std::ifstream in(MaterialFileName);
	if (!in)
	{
		return false;
	}


	
	std::string line, word;
	UINT index;

	//read number of vertex in the first line (three of them is a triangle)
	while (!in.eof())
	{
		getline(in, line);

		//if current line contains "newmtl", find its material
		if (line.find("newmtl") != std::string::npos)
		{
			std::istringstream record(line);
			record >> word;
			record >> word;

			//traversal whole array, find the model which have corresponding material
			for (size_t i = 0; i < m_ObjModelArray.size(); ++i)
			{
				if (word == m_ObjModelArray[i]->GetMaterialName())
				{
					index = i;
					break;
				}
			}

			//enter inner circle£¬ read property of respond material
			while (1)
			{

				getline(in, line);

				//specular power
				if (line.find("Ns") != std::string::npos)
				{
					std::istringstream re(line);
					re >> word;
					re >> word;
					float SpecularPower = atof(&word[0]);
					m_ObjModelArray[index]->SetSpecularPower(SpecularPower);
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
					m_ObjModelArray[index]->SetAmbientMaterial(r, g, b);
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
					m_ObjModelArray[index]->SetDiffuseMaterial(r, g, b);

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
					m_ObjModelArray[index]->SetSpecularMaterial(r, g, b);
				}
				//
				else if (line.find("Ke") != std::string::npos)
				{
					//break inner circle
					break;
				}

			}
		}

	}

	return true;
}




