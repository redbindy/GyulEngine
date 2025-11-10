#include "MeshManager.h"

#include "Mesh.h"
#include "Core/LogHelper.h"
#include "Shape.h"

enum
{
	DEFAULT_BUFFER_SIZE = 32
};

MeshManager* MeshManager::spInstance = nullptr;

MeshManager::MeshManager(ID3D11Device& device)
	: mDevice(device)
	, mMeshMap()
{
	mMeshMap.reserve(DEFAULT_BUFFER_SIZE);

	{
		std::vector<Vertex::PosNormalUV> vertices;
		std::vector<int16_t> indices;

		Shape::CreateTriangleDataAlloc(vertices, indices);

		CreateMesh("Triangle", vertices, indices);
	}

	{
		std::vector<Vertex::PosNormalUV> vertices;
		std::vector<int16_t> indices;

		Shape::CreateSquareDataAlloc(vertices, indices);

		CreateMesh("Square", vertices, indices);
	}

	{
		std::vector<Vertex::PosNormalUV> vertices;
		std::vector<int16_t> indices;

		Shape::CreateCubeDataAlloc(vertices, indices);

		CreateMesh("Cube", vertices, indices);
	}

	{
		std::vector<Vertex::PosNormalUV> vertices;
		std::vector<int16_t> indices;

		Shape::CreateSphereDataAlloc(vertices, indices);

		CreateMesh("Sphere", vertices, indices);
	}
}

MeshManager::~MeshManager()
{
	for (std::pair<const std::string, Mesh*>& pair : mMeshMap)
	{
		delete pair.second;
	}
}

Mesh* MeshManager::CreateMesh(
	const std::string& path,
	const std::vector<Vertex::PosNormalUV>& vertices,
	const std::vector<int16_t>& indices
)
{
	const float radius = calculateBoundingSphereLocalRadius(vertices);

	return createMeshAlloc(
		path,
		Vertex::EType::POS_NORMAL_UV,
		vertices.data(),
		static_cast<UINT>(vertices.size()),
		sizeof(Vertex::PosNormalUV),
		indices.data(),
		static_cast<UINT>(indices.size()),
		sizeof(int16_t),
		radius);
}

Mesh* MeshManager::CreateMesh(
	const std::string& path,
	const std::vector<Vertex::PosNormalUV>& vertices,
	const std::vector<int32_t>& indices
)
{
	const float radius = calculateBoundingSphereLocalRadius(vertices);

	return createMeshAlloc(
		path,
		Vertex::EType::POS_NORMAL_UV,
		vertices.data(),
		static_cast<UINT>(vertices.size()),
		sizeof(Vertex::PosNormalUV),
		indices.data(),
		static_cast<UINT>(indices.size()),
		sizeof(int32_t),
		radius);
}

Mesh* MeshManager::GetMeshOrNull(const std::string& path) const
{
#define MAP_ITER std::unordered_map<std::string, Mesh*>::const_iterator

	MAP_ITER iter = mMeshMap.find(path);

	if (iter != mMeshMap.end())
	{
		return iter->second;
	}

#undef MAP_ITER

	return nullptr;
}

void MeshManager::UnloadMesh(const std::string& path)
{
#define MAP_ITER std::unordered_map<std::string, Mesh*>::iterator

	MAP_ITER iter = mMeshMap.find(path);

	if (iter != mMeshMap.end())
	{
		delete iter->second;

		mMeshMap.erase(iter);
	}

#undef MAP_ITER
}

void MeshManager::Initialize(ID3D11Device& device)
{
	ASSERT(spInstance == nullptr);

	spInstance = new MeshManager(device);
}

float MeshManager::calculateBoundingSphereLocalRadius(const std::vector<Vertex::PosNormalUV>& vertices)
{
	Vector3 minPos(D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX);
	Vector3 maxPos = -minPos;

	for (const Vertex::PosNormalUV& vertex : vertices)
	{
		minPos = Vector3::Min(minPos, vertex.pos);
		maxPos = Vector3::Max(maxPos, vertex.pos);
	}

	const float radius = Vector3::Distance(minPos, maxPos) * 0.5f;

	return radius;
}

Mesh* MeshManager::createMeshAlloc(
	const std::string& path,
	const Vertex::EType eVertexType,
	const void* pVertexData,
	const UINT vertexCount,
	const UINT vertexStride,
	const void* pIndexData,
	const UINT indexCount,
	const UINT indexStride,
	const float radius
)
{
#define MAP_ITER std::unordered_map<std::string, Mesh*>::const_iterator

	MAP_ITER iter = mMeshMap.find(path);

	if (iter != mMeshMap.end())
	{
		return iter->second;
	}

#undef MAP_ITER

	ComPtr<ID3D11Buffer> vertexBufferPtr;
	{
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = vertexCount * vertexStride;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));

		initData.pSysMem = pVertexData;

		HRESULT hr = mDevice.CreateBuffer(&bufferDesc, &initData, vertexBufferPtr.GetAddressOf());

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreateBuffer - VertexBuffer");

			ASSERT(false);
		}
	}

	ComPtr<ID3D11Buffer> indexBufferPtr;
	{
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.ByteWidth = indexCount * indexStride;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA initData;
		ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));

		initData.pSysMem = pIndexData;

		HRESULT hr = mDevice.CreateBuffer(&bufferDesc, &initData, indexBufferPtr.GetAddressOf());

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreateBuffer - IndexBuffer");

			ASSERT(false);
		}
	}

	Mesh* const pMesh = new Mesh(
		path,
		eVertexType,
		vertexBufferPtr,
		vertexCount,
		vertexStride,
		indexBufferPtr,
		indexCount,
		indexStride,
		radius
	);

	mMeshMap.insert(std::make_pair(path, pMesh));

	return pMesh;
}