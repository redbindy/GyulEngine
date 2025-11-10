#pragma once

#include <string>
#include <d3d11.h>

#include "Core/MathHelper.h"
#include "Core/ComHelper.h"
#include "Core/Assert.h"
#include "Renderer/Vertex.h"

class Mesh final
{
public:
	Mesh(
		const std::string& path,
		const Vertex::EType eVertexType,
		ComPtr<ID3D11Buffer>& vertexBufferPtr,
		const UINT vertexCount,
		const UINT vertexStride,
		ComPtr<ID3D11Buffer>& indexBufferPtr,
		const UINT indexCount,
		const UINT indexStride,
		const float radius
	);
	~Mesh() = default;

	void Bind(ID3D11DeviceContext& deviceContext) const;

	inline const char* GetPath() const
	{
		return mPath.c_str();
	}

	inline UINT GetIndexCount() const
	{
		return mIndexCount;
	}

	inline const BoundingSphere& GetBoundingSphereLocal() const
	{
		return mBoundingSphereLocal;
	}

private:
	std::string mPath;

	Vertex::EType mVertexType;

	ComPtr<ID3D11Buffer> mpVertexBuffer;
	UINT mVertexCount;
	UINT mVertexStride;

	ComPtr<ID3D11Buffer> mpIndexBuffer;
	UINT mIndexCount;
	UINT mIndexStride;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveTopology;

	BoundingSphere mBoundingSphereLocal;

private:
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;
};