#include "ModelManager.h"

#include <filesystem>

#include "Core/Assert.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"
#include "Core/FileDialog.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "MaterialManager.h"
#include "Shape.h"

enum
{
	DEFAULT_BUFFER_SIZE = 32
};

ModelManager* ModelManager::spInstance = nullptr;

ModelManager::ModelManager()
	: mLoadedModels()
{
	mLoadedModels.reserve(DEFAULT_BUFFER_SIZE);

	MeshManager& meshManager = MeshManager::GetInstance();
	MaterialManager& materialManager = MaterialManager::GetInstance();

	Material* const pDefaultMaterial = materialManager.GetMaterialOrNull("Default");

	{
		std::vector<Vertex::PosNormalUV> vertices;
		std::vector<uint16_t> indices;

		Shape::CreateTriangleDataAlloc(vertices, indices);

		Mesh* const pTriangle = meshManager.CreateMesh("Triangle", vertices, indices);

		const float boundingRadius = calculateBoundingSphereLocalRadius(vertices);

		ModelData modelData
		{
			{ pTriangle, pDefaultMaterial }
		};

		Model* const pTriangleModel = new Model("Triangle", modelData, Vector3::Zero, boundingRadius);

		mLoadedModels.insert(std::make_pair("Triangle", pTriangleModel));
	}

	{
		std::vector<Vertex::PosNormalUV> vertices;
		std::vector<uint16_t> indices;

		Shape::CreateSquareDataAlloc(vertices, indices);

		Mesh* const pSquare = meshManager.CreateMesh("Square", vertices, indices);

		const float boundingRadius = calculateBoundingSphereLocalRadius(vertices);

		ModelData modelData =
		{
			{ pSquare, pDefaultMaterial  }
		};

		Model* const pSquareModel = new Model("Square", modelData, Vector3::Zero, boundingRadius);

		mLoadedModels.insert(std::make_pair("Square", pSquareModel));
	}

	{
		std::vector<Vertex::PosNormalUV> vertices;
		std::vector<uint16_t> indices;

		Shape::CreateCubeDataAlloc(vertices, indices);

		Mesh* const pCube = meshManager.CreateMesh("Cube", vertices, indices);

		const float boundingRadius = calculateBoundingSphereLocalRadius(vertices);

		ModelData modelData =
		{
			{ pCube, pDefaultMaterial }
		};

		Model* const pCubeModel = new Model("Cube", modelData, Vector3::Zero, boundingRadius);

		mLoadedModels.insert(std::make_pair("Cube", pCubeModel));
	}

	{
		std::vector<Vertex::PosNormalUV> vertices;
		std::vector<uint16_t> indices;

		Shape::CreateSphereDataAlloc(vertices, indices);

		Mesh* const pSphere = meshManager.CreateMesh("Sphere", vertices, indices);

		const float boundingRadius = calculateBoundingSphereLocalRadius(vertices);

		ModelData modelData =
		{
			{ pSphere, pDefaultMaterial }
		};

		Model* const pSphereModel = new Model("Sphere", modelData, Vector3::Zero, boundingRadius);

		mLoadedModels.insert(std::make_pair("Sphere", pSphereModel));
	}
}

ModelManager::~ModelManager()
{
	for (std::pair<const std::string, Model*>& pair : mLoadedModels)
	{
		delete pair.second;
	}
}

void ModelManager::Load(const std::string& path)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(
		path,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded
	);

	Matrix tr;
	ModelData modelData;

	Vector3 center = Vector3::Zero;
	int totalVertices = 0;

	Vector3 minVector(D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX);
	Vector3 maxVector = -minVector;

	processNodeRecursive(
		pScene->mRootNode,
		pScene,
		tr,
		modelData,
		center,
		totalVertices,
		minVector,
		maxVector
	);

	center /= static_cast<float>(totalVertices);
	const float boundingRadius = (maxVector - minVector).Length() * 0.5f;

	Model* pModel = new Model(path, modelData, center, boundingRadius);

	mLoadedModels.insert(std::make_pair(path, pModel));
}

Model* ModelManager::GetModelOrNull(const std::string& path)
{
	if (mLoadedModels.find(path) != mLoadedModels.end())
	{
		return mLoadedModels[path];
	}

	return nullptr;
}

void ModelManager::Unload(const std::string& path)
{
	if (mLoadedModels.find(path) != mLoadedModels.end())
	{
		delete mLoadedModels[path];

		mLoadedModels.erase(path);
	}
}

void ModelManager::processNodeRecursive(
	aiNode* pNode,
	const aiScene* pScene,
	const Matrix tr,
	ModelData& outModelData,
	Vector3& outCenter,
	int& totalVertices,
	Vector3& outMinVector,
	Vector3& outMaxVector
)
{
	Matrix m;
	ai_real* pTmp = &pNode->mTransformation.a1;
	float* pMTmp = &m._11;

	for (int t = 0; t < 16; ++t)
	{
		pMTmp[t] = static_cast<float>(pTmp[t]);
	}
	m = m.Transpose() * tr;

	for (UINT i = 0; i < pNode->mNumMeshes; ++i)
	{
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];

		// Process Mesh
		std::vector<Vertex::PosNormalUV> vertices;
		vertices.reserve(pMesh->mNumVertices);

		totalVertices += pMesh->mNumVertices;
		for (UINT j = 0; j < pMesh->mNumVertices; ++j)
		{
			Vertex::PosNormalUV vertex;

			vertex.pos.x = pMesh->mVertices[j].x;
			vertex.pos.y = pMesh->mVertices[j].y;
			vertex.pos.z = pMesh->mVertices[j].z;

			vertex.pos = Vector3::Transform(vertex.pos, m);

			outCenter += vertex.pos;
			outMinVector = Vector3::Min(outMinVector, vertex.pos);
			outMaxVector = Vector3::Max(outMaxVector, vertex.pos);

			vertex.normal.x = pMesh->mNormals[j].x;
			vertex.normal.y = pMesh->mNormals[j].y;
			vertex.normal.z = pMesh->mNormals[j].z;
			vertex.normal.Normalize();

			if (pMesh->mTextureCoords[0])
			{
				vertex.uv.x = static_cast<float>(pMesh->mTextureCoords[0][j].x);
				vertex.uv.y = static_cast<float>(pMesh->mTextureCoords[0][j].y);
			}

			vertices.push_back(vertex);
		}

		std::vector<uint32_t> indices;
		indices.reserve(pMesh->mNumVertices * 3);

		for (UINT j = 0; j < pMesh->mNumFaces; ++j)
		{
			aiFace face = pMesh->mFaces[j];
			for (UINT k = 0; k < face.mNumIndices; ++k)
			{
				indices.push_back(face.mIndices[k]);
			}
		}

		// Only use baseColor texture
		std::string texturePath = "./Assets/";
		if (pMesh->mMaterialIndex >= 0)
		{
			aiMaterial* pMaterial = pScene->mMaterials[pMesh->mMaterialIndex];

			aiString aiMaterialName;

			if (pMaterial->Get(AI_MATKEY_NAME, aiMaterialName) == AI_SUCCESS)
			{
				texturePath.append("miyako/textures/i_");
				texturePath.append(aiMaterialName.C_Str());
				texturePath.append("_BaseColor.png");
			}

			std::filesystem::path fsTexturePath(texturePath);
			if (std::filesystem::exists(fsTexturePath))
			{
				texturePath = fsTexturePath.string();

				TextureManager& textureManager = TextureManager::GetInstance();
				textureManager.LoadTexture(texturePath);
			}
			else
			{
				texturePath.append("Default.dds");
			}
		}

		std::string key = texturePath;

		key.append(std::to_string(outModelData.size()));
		MeshManager& meshManager = MeshManager::GetInstance();
		Mesh* pMeshGenerated = meshManager.CreateMesh(
			key,
			vertices,
			indices
		);

		MaterialManager& materialManager = MaterialManager::GetInstance();
		Material* pMaterialGenerated = materialManager.CreateMaterial(
			key,
			texturePath,
			"./Shaders/VSBasic.hlsl",
			"./Shaders/PSBasic.hlsl"
		);

		outModelData.push_back(std::make_pair(pMeshGenerated, pMaterialGenerated));
	}

	for (UINT i = 0; i < pNode->mNumChildren; ++i)
	{
		processNodeRecursive(
			pNode->mChildren[i],
			pScene,
			m,
			outModelData,
			outCenter,
			totalVertices,
			outMinVector,
			outMaxVector
		);
	}
}

float ModelManager::calculateBoundingSphereLocalRadius(const std::vector<Vertex::PosNormalUV>& vertices)
{
	Vector3 minVector(D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX);
	Vector3 maxVector = -minVector;

	for (const Vertex::PosNormalUV& vertex : vertices)
	{
		minVector = Vector3::Min(minVector, vertex.pos);
		maxVector = Vector3::Max(maxVector, vertex.pos);
	}

	const float radius = (maxVector - minVector).Length() * 0.5f;

	return radius;
}

void ModelManager::DrawEditorUI()
{
	ImGui::PushID("ModelManager");

	ImGui::Text(UTF8_TEXT("¸ðµ¨"));

	if (ImGui::Button(UTF8_TEXT("¸ðµ¨ ºÒ·¯¿À±â")))
	{
		FileDialog& fileDialog = FileDialog::GetInstance();

		char filePath[FileDialog::PATH_BUFFER_SIZE];
		if (fileDialog.TryOpenFileDialog(filePath, FileDialog::PATH_BUFFER_SIZE))
		{
			Load(filePath);
		}
	}

	for (const std::pair<const std::string, Model*>& pair : mLoadedModels)
	{
		ImGui::Text(UTF8_TEXT("%s"), pair.first.c_str());
	}

	ImGui::PopID();
}

bool ModelManager::DrawModelSelectorPopupAndSelectModel(Model*& pOutSelectedModel)
{
	ImGui::PushID("ModelManager");

	const char* const popupName = UTF8_TEXT("¸ðµ¨ ¸ñ·Ï");

	ImGui::OpenPopup(popupName);

	// selecet or cancel -> true
	bool bRet = false;

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		for (const std::pair<const std::string, Model*>& pair : mLoadedModels)
		{
			if (ImGui::Button(pair.first.c_str()))
			{
				pOutSelectedModel = pair.second;

				bRet = true;
			}
		}

		ImGui::EndPopup();
	}

	ImGui::PopID();

	return bRet || ImGui::IsKeyPressed(ImGuiKey_Escape);
}

void ModelManager::Initialize()
{
	ASSERT(spInstance == nullptr);

	spInstance = new ModelManager();
}
