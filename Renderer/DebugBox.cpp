#include "DebugSphere.h"

#include <cstdint>
#include <vector>

#include "CommonDefs.h"
#include "ComHelper.h"
#include "Vertex.h"
#include "Renderer.h"

DebugSphere::DebugSphere()
	: mCenter(0.f, 0.f, 0.f)
	, mScale(1.f)
	, mpVertexBuffer(nullptr)
	, mpIndexBuffer(nullptr)
	, mpInputLayout(nullptr)
	, mPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, mpVertexShader(nullptr)
	, mpPixelShader(nullptr)
	, mIndexCount(-1)
{
	constexpr float radius = 1.f;
	constexpr int32_t sliceCount = 20;
	constexpr int32_t stackCount = 20;

	std::vector<Vertex> vertices;
	vertices.reserve((stackCount + 1) * (sliceCount + 1));

	std::vector<int16_t> indices;
	indices.reserve(vertices.capacity() * 6);

	constexpr float DELTA_THETA = -XM_2PI / sliceCount;
	constexpr float DELTA_PHI = -XM_PI / stackCount;

	const Vector3 startPoint(0.f, -radius, 0.f);
	for (int y = 0; y <= stackCount; ++y)
	{
		const Matrix rotationZ = Matrix::CreateRotationZ(DELTA_PHI * y);
		const Vector3 sliceStart = Vector3::Transform(startPoint, rotationZ);

		for (int x = 0; x <= sliceCount; ++x)
		{
			const Matrix rotationY = Matrix::CreateRotationY(DELTA_THETA * x);

			const Vector3 currPoint = Vector3::Transform(sliceStart, rotationY);

			const Vertex vertex = { currPoint, { 0.f, 0.f, 0.f }, { 0.f, 0.f } };

			vertices.push_back(vertex);
		}
	}

	for (int y = 0; y < stackCount; ++y)
	{
		const int start = (sliceCount + 1) * y;
		for (int x = 0; x < sliceCount; ++x)
		{
			indices.push_back(start + x);
			indices.push_back(start + x + sliceCount + 1);
			indices.push_back(start + x + 1 + sliceCount + 1);

			indices.push_back(start + x);
			indices.push_back(start + x + 1 + sliceCount + 1);
			indices.push_back(start + x + 1);
		}
	}

	mIndexCount = static_cast<int>(indices.size());

	Renderer& renderer = Renderer::GetInstance();

	bool bResult = renderer.TryCreateBuffer(
		EBufferType::VERTEX,
		vertices.data(),
		static_cast<UINT>(vertices.size()) * sizeof(Vertex),
		0,
		mpVertexBuffer
	);
	ASSERT(bResult);

	bResult = renderer.TryCreateBuffer(
		EBufferType::INDEX,
		indices.data(),
		static_cast<UINT>(indices.size()) * sizeof(uint16_t),
		0,
		mpIndexBuffer
	);
	ASSERT(bResult);

	mpInputLayout = renderer.GetInputLayout(EVertexType::POS_NORMAL_UV);
	ASSERT(mpInputLayout != nullptr);

	mpVertexShader = renderer.GetVertexShaderOrNull(SHADER_PATH("VSBoundingSphere.hlsl"));
	ASSERT(mpVertexShader != nullptr);

	mpPixelShader = renderer.GetPixelShaderOrNull(SHADER_PATH("PSBoundingSphere.hlsl"));
	ASSERT(mpPixelShader != nullptr);

	mpBlendState = renderer.GetBlendState();
	ASSERT(mpBlendState != nullptr);
}

DebugSphere::~DebugSphere()
{
	SafeRelease(mpIndexBuffer);
	SafeRelease(mpVertexBuffer);
}

void DebugSphere::Draw(ID3D11DeviceContext& deviceContext)
{
	deviceContext.IASetInputLayout(mpInputLayout);
	deviceContext.IASetPrimitiveTopology(mPrimitiveTopology);

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	deviceContext.IASetVertexBuffers(0, 1, &mpVertexBuffer, &stride, &offset);
	deviceContext.IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	deviceContext.VSSetShader(mpVertexShader, nullptr, 0);
	deviceContext.PSSetShader(mpPixelShader, nullptr, 0);

	const Matrix scale = Matrix::CreateScale(mScale);
	const Matrix translation = Matrix::CreateTranslation(mCenter);

	const Matrix worldMat = scale * translation;

	Renderer& renderer = Renderer::GetInstance();
	renderer.UpdateCBWorldMatrix({ worldMat.Transpose() });

	constexpr UINT SAMPLE_MASK = 0xFFFFFFFF;
	deviceContext.OMSetBlendState(mpBlendState, nullptr, SAMPLE_MASK);

	deviceContext.DrawIndexed(mIndexCount, 0, 0);

	deviceContext.OMSetBlendState(nullptr, nullptr, SAMPLE_MASK);
}
