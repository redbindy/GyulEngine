#pragma once

#include <string>
#include <unordered_map>

#include <d3d11.h>

#include "Core/Assert.h"
#include "Renderer/Vertex.h"
#include "UI/IEditorUIDrawable.h"

class Mesh;

class MeshManager final : public IEditorUIDrawable
{
public:
	Mesh* CreateMesh(
		const std::string& path,
		const std::vector<Vertex::PosNormalUV>& vertices,
		const std::vector<int16_t>& indices
	);

	Mesh* CreateMesh(
		const std::string& path,
		const std::vector<Vertex::PosNormalUV>& vertices,
		const std::vector<int32_t>& indices
	);

	Mesh* GetMeshOrNull(const std::string& path) const;
	void UnloadMesh(const std::string& path);

	virtual void DrawEditorUI() override;

	// static
	static void Initialize(ID3D11Device& device);

	static MeshManager& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static MeshManager* spInstance;

	ID3D11Device& mDevice;

	std::unordered_map<std::string, Mesh*> mMeshMap;

private:
	MeshManager(ID3D11Device& device);
	~MeshManager();

	float calculateBoundingSphereLocalRadius(const std::vector<Vertex::PosNormalUV>& vertices);

	Mesh* createMeshAlloc(
		const std::string& path,
		const Vertex::EType eVertexType,
		const void* pVertexData,
		const UINT vertexCount,
		const UINT vertexStride,
		const void* pIndexData,
		const UINT indexCount,
		const UINT indexStride,
		const float radius);

private:
	MeshManager(const MeshManager& other) = delete;
	MeshManager(MeshManager&& other) = delete;
	MeshManager& operator=(const MeshManager& other) = delete;
	MeshManager& operator=(MeshManager&& other) = delete;
};