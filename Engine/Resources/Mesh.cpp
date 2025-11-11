#include "Mesh.h"

#include "ShaderManager.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"

Mesh::Mesh(
	const std::string& path,
	const Vertex::EType eVertexType,
	ComPtr<ID3D11Buffer>& vertexBufferPtr,
	const UINT vertexCount,
	const UINT vertexStride,
	ComPtr<ID3D11Buffer>& indexBufferPtr,
	const UINT indexCount,
	const UINT indexStride,
	const float radius
)
	: mPath(path)
	, mVertexType(eVertexType)
	, mpVertexBuffer(vertexBufferPtr)
	, mVertexCount(vertexCount)
	, mVertexStride(vertexStride)
	, mpIndexBuffer(indexBufferPtr)
	, mIndexCount(indexCount)
	, mIndexStride(indexStride)
	, mPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, mBoundingSphereLocal(Vector3(0.f, 0.f, 0.f), radius)
{
	ASSERT(vertexBufferPtr != nullptr);
	ASSERT(indexBufferPtr != nullptr);
}

void Mesh::Bind(ID3D11DeviceContext& deviceContext) const
{
	const UINT offset = 0;

	ShaderManager& shaderManager = ShaderManager::GetInstance();

	ID3D11InputLayout* const pInputLayout = shaderManager.GetInputLayoutOrNull(mVertexType);
	ASSERT(pInputLayout != nullptr);

	deviceContext.IASetInputLayout(pInputLayout);
	deviceContext.IASetVertexBuffers(0, 1, mpVertexBuffer.GetAddressOf(), &mVertexStride, &offset);

	if (mIndexStride == sizeof(int16_t))
	{
		deviceContext.IASetIndexBuffer(mpIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	}
	else
	{
		deviceContext.IASetIndexBuffer(mpIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	deviceContext.IASetPrimitiveTopology(mPrimitiveTopology);
}

void Mesh::DrawEditorUI()
{
	ImGui::SeparatorText(UTF8_TEXT("¸Þ½¬"));

	ImGui::Text("Path: %s", mPath.c_str());
	ImGui::Text("Vertex Type: %s", Vertex::GetTypeName(mVertexType));
	ImGui::Text("Vertex Count: %u", mVertexCount);
	ImGui::Text("Index Count: %u", mIndexCount);

	const char* topologyName = "Unknown";
	switch (mPrimitiveTopology)
	{
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		topologyName = "Triangle List";
		break;

	default:
		ASSERT(false);
		break;
	}

	ImGui::Text("Topology: %s", topologyName);
}
