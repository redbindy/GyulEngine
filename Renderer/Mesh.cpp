#include "Mesh.h"

#include "ComHelper.h"
#include "Renderer.h"

Mesh::Mesh(
	const Vertex* const vertices,
	const UINT vertexCount,
	const int32_t* const indices,
	const UINT indexCount
)
	: mpVertexBufferGPU(nullptr)
	, mpIndexBufferGPU(nullptr)
	, mVertexType(EVertexType::POS_NORMAL_UV)
	, mpInputLayout(nullptr)
	, mPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, mVertexCount(vertexCount)
	, mIndexCount(indexCount)
{
	Renderer* const pRenderer = Renderer::GetInstance();
	ASSERT(pRenderer != nullptr);

	bool bResult = pRenderer->TryCreateBuffer(
		EBufferType::VERTEX,
		vertices,
		sizeof(Vertex) * mVertexCount,
		sizeof(Vertex),
		mpVertexBufferGPU
	);
	ASSERT(bResult);

	bResult = pRenderer->TryCreateBuffer(
		EBufferType::INDEX,
		indices,
		sizeof(int32_t) * mIndexCount,
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

Mesh* Shape::CreateTriangleAlloc()
{
	constexpr int VERTEX_COUNT = 3;

	constexpr Vertex vertices[VERTEX_COUNT] = {
			{ { 0.f, 0.5f, 0.f }, { 0.f, 0.f, -1.f }, { 0.5f, 0.f } },
			{ { 0.5f, -0.5f, 0.f }, { 0.f, 0.f, -1.f }, { 1.f, 1.f } },
			{ { -0.5f, -0.5f, 0.f }, { 0.f, 0.f, -1.f }, { 0.f, 1.f } }
	};

	constexpr int32_t indices[VERTEX_COUNT] = { 0, 1, 2 };

	return new Mesh(vertices, VERTEX_COUNT, indices, VERTEX_COUNT);
}

Mesh* Shape::CreateCubeAlloc()
{

	Vertex vertices[] = {
		// À­¸é
		{ { -1.f, 1.f, -1.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f } },
		{ { -1.f, 1.f, 1.f }, { 0.f, 1.f, 0.f }, { 1.f, 0.f } },
		{ { 1.f, 1.f, 1.f }, { 0.f, 1.f, 0.f }, { 1.f, 1.f } },
		{ { 1.f, 1.f, -1.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f } },

		// ¾Æ·§¸é
		{ { -1.f, -1.f, -1.f }, { 0.f, -1.f, 0.f }, { 0.f, 0.f } },
		{ { 1.f, -1.f, -1.f }, { 0.f, -1.f, 0.f }, { 1.f, 0.f } },
		{ { 1.f, -1.f, 1.f }, { 0.f, -1.f, 0.f }, { 1.f, 1.f } },
		{ { -1.f, -1.f, 1.f }, { 0.f, -1.f, 0.f }, { 0.f, 1.f } },

		// ¾Õ¸é
		{ { -1.f, -1.f, -1.f }, { 0.f, 0.f, -1.f }, { 0.f, 0.f } },
		{ { -1.f, 1.f, -1.f }, { 0.f, 0.f, -1.f }, { 1.f, 0.f } },
		{ { 1.f, 1.f, -1.f }, { 0.f, 0.f, -1.f }, { 1.f, 1.f } },
		{ { 1.f, -1.f, -1.f }, { 0.f, 0.f, -1.f }, { 0.f, 1.f } },

		// µÞ¸é
		{ { -1.f, -1.f, 1.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f } },
		{ { 1.f, -1.f, 1.f }, { 0.f, 0.f, 1.f }, { 1.f, 0.f } },
		{ { 1.f, 1.f, 1.f }, { 0.f, 0.f, 1.f }, { 1.f, 1.f } },
		{ { -1.f, 1.f, 1.f }, { 0.f, 0.f, 1.f }, { 0.f, 1.f } },

		// ¿ÞÂÊ
		{ { -1.f, -1.f, 1.f }, { -1.f, 1.f, 0.f }, { 0.f, 0.f } },
		{ { -1.f, 1.f, 1.f }, { -1.f, 1.f, 0.f }, { 1.f, 0.f } },
		{ { -1.f, 1.f, -1.f }, { -1.f, 1.f, 0.f }, { 1.f, 1.f } },
		{ { -1.f, -1.f, -1.f }, { -1.f, 1.f, 0.f }, { 0.f, 1.f } },

		// ¿À¸¥ÂÊ
		{ { 1.f, -1.f, 1.f }, { 1.f, 0.f, 0.f }, { 0.f, 0.f } },
		{ { 1.f, -1.f, -1.f }, { 1.f, 0.f, 0.f }, { 1.f, 0.f } },
		{ { 1.f, 1.f, -1.f }, { 1.f, 0.f, 0.f }, { 1.f, 1.f } },
		{ { 1.f, 1.f, 1.f }, { 1.f, 0.f, 0.f }, { 0.f, 1.f } }
	};

	constexpr UINT vertexCount = static_cast<UINT>(ARRAYSIZE(vertices));

	int32_t indices[] = {
		// À­¸é
		0,  1,  2,
		0,  2,  3,

		// ¾Æ·§¸é
		4,  5,  6,
		4,  6,  7,

		// ¾Õ¸é
		8,  9,  10,
		8,  10, 11,

		// µÞ¸é
		12, 13, 14,
		12, 14, 15,

		// ¿ÞÂÊ
		16, 17, 18,
		16, 18, 19,

		// ¿À¸¥ÂÊ
		20, 21, 22,
		20, 22, 23
	};

	constexpr UINT indexCount = static_cast<UINT>(ARRAYSIZE(indices));

	return new Mesh(vertices, vertexCount, indices, indexCount);
}
