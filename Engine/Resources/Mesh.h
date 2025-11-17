#pragma once

#include <string>
#include <d3d11.h>

#include "Core/MathHelper.h"
#include "Core/ComHelper.h"
#include "Core/Assert.h"
#include "Renderer/Vertex.h"
#include "UI/IEditorUIDrawable.h"

class Mesh final : public IEditorUIDrawable
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
		const UINT indexStride
	);
	~Mesh() = default;

	void Bind(ID3D11DeviceContext& deviceContext) const;

	virtual void DrawEditorUI() override;

	inline const char* GetPath() const
	{
		return mPath.c_str();
	}

	inline UINT GetIndexCount() const
	{
		return mIndexCount;
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

private:
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) = delete;
	Mesh& operator=(const Mesh& other) = delete;
	Mesh& operator=(Mesh&& other) = delete;
};