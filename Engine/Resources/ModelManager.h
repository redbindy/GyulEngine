#pragma once

#include <unordered_map>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "UI/IEditorUIDrawable.h"
#include "Core/MathHelper.h"
#include "Model.h"
#include "Renderer/Vertex.h"

class Mesh;
class Material;

class ModelManager final : public IEditorUIDrawable
{
public:
	void Load(const std::string& path);
	Model* GetModelOrNull(const std::string& path);
	void Unload(const std::string& path);

	virtual void DrawEditorUI() override;

	bool DrawModelSelectorPopupAndSelectModel(Model*& pOutSelectedModel);

	static void Initialize();

	static ModelManager& GetInstance()
	{
		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static ModelManager* spInstance;

	std::unordered_map<std::string, Model*> mLoadedModels;

private:
	ModelManager();
	~ModelManager();

	void processNodeRecursive(
		aiNode* pNode,
		const aiScene* pScene,
		const Matrix tr,
		ModelData& outModelData,
		Vector3& outCenter,
		int& totalVertices,
		Vector3& outMinVector,
		Vector3& outMaxVector
	);

	float calculateBoundingSphereLocalRadius(const std::vector<Vertex::PosNormalUV>& vertices);

private:
	ModelManager(const ModelManager& other) = delete;
	ModelManager& operator=(const ModelManager& other) = delete;
	ModelManager(ModelManager&& other) = delete;
	ModelManager& operator=(ModelManager&& other) = delete;
};
