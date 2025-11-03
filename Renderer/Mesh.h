#pragma once

#include <d3d11.h>

#include "Vertex.h"
#include "IUIDrawable.h"

class Mesh final : public IUIDrawable
{
public:
	Mesh(
		const Vertex* const vertices,
		const UINT vertexCount,
		const int32_t* const indices,
		const UINT indexCount
	);
	~Mesh();
	Mesh(const Mesh& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;

	void Bind(ID3D11DeviceContext& deviceContext) const;

	virtual void DrawUI() override;

	inline UINT GetIndexCount() const
	{
		return mIndexCount;
	}

	BoundingSphere GetBoundingSphereLocal() const
	{
		return mBoundingSphereLocal;
	}

private:
	ID3D11Buffer* mpVertexBufferGPU;
	ID3D11Buffer* mpIndexBufferGPU;
	UINT mVertexCount;
	UINT mIndexCount;

	EVertexType mVertexType;
	ID3D11InputLayout* mpInputLayout;
	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveTopology;

	BoundingSphere mBoundingSphereLocal;
};

namespace Shape
{
	Mesh* CreateTriangleAlloc();
	Mesh* CreateCubeAlloc();
	Mesh* CreateSphereAlloc();
	Mesh* CreateSquareAlloc();
};