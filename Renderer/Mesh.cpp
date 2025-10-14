#include "Mesh.h"

#include "ComHelper.h"
#include "Renderer.h"

Mesh::Mesh()
	: mpVertexBufferGPU(nullptr)
	, mpIndexBufferGPU(nullptr)
	, mVertexType(EVertexType::POS_NORMAL_UV)
	, mpInputLayout(nullptr)
	, mPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, mVertexCount(3)
	, mIndexCount(3)
{
	constexpr int VERTEX_COUNT = 3;

	constexpr Vertex vertices[VERTEX_COUNT] = {
			{ { 0.f, 0.5f, 0.f }, { 0.f, 0.f, -1.f }, { 0.5f, 0.f } },
			{ { 0.5f, -0.5f, 0.f }, { 0.f, 0.f, -1.f }, { 1.f, 1.f } },
			{ { -0.5f, -0.5f, 0.f }, { 0.f, 0.f, -1.f }, { 0.f, 1.f } }
	};

	constexpr int32_t indices[VERTEX_COUNT] = { 0, 1, 2 };

	Renderer* const pRenderer = Renderer::GetInstance();
	ASSERT(pRenderer != nullptr);

	bool bResult = pRenderer->TryCreateBuffer(
		EBufferType::VERTEX,
		vertices,
		sizeof(Vertex) * VERTEX_COUNT,
		sizeof(Vertex),
		mpVertexBufferGPU
	);
	ASSERT(bResult);

	bResult = pRenderer->TryCreateBuffer(
		EBufferType::INDEX,
		indices,
		sizeof(int32_t) * VERTEX_COUNT,
		sizeof(int32_t),
		mpIndexBufferGPU
	);
	ASSERT(bResult);

	mpInputLayout = pRenderer->GetInputLayout(EVertexType::POS_NORMAL_UV);
	ASSERT(mpInputLayout != nullptr);
}

Mesh::~Mesh()
{
	SafeRelease(mpIndexBufferGPU);
	SafeRelease(mpVertexBufferGPU);
}

void Mesh::Bind(ID3D11DeviceContext& deviceContext) const
{
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;

	deviceContext.IASetVertexBuffers(0, 1, &mpVertexBufferGPU, &stride, &offset);
	deviceContext.IASetIndexBuffer(mpIndexBufferGPU, DXGI_FORMAT_R32_UINT, offset);
	deviceContext.IASetPrimitiveTopology(mPrimitiveTopology);
	deviceContext.IASetInputLayout(mpInputLayout);
}

void Mesh::DrawUI()
{
	if (ImGui::TreeNodeEx("Mesh", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Vertices: %d", mVertexCount);
		ImGui::Text("Indices: %d", mIndexCount);
		ImGui::Text("VertexType: %s", GetVertexTypeString(mVertexType));

		const char* topologyString = nullptr;
		switch (mPrimitiveTopology)
		{
		case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			topologyString = "TriangleList";
			break;

		default:
			topologyString = "";
			break;
		}

		ImGui::Text("Topology: %s", topologyString);

		ImGui::TreePop();
	}
}
