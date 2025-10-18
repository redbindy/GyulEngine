#include "DebugSphere.h"

#include <cstdint>
#include <vector>

#include "CommonDefs.h"
#include "ComHelper.h"
#include "Vertex.h"
#include "Renderer.h"
#include "Mesh.h"

DebugSphere::DebugSphere()
	: mCenter(0.f, 0.f, 0.f)
	, mScale(1.f)
	, mpVertexShader(nullptr)
	, mpWireframeState(nullptr)
	, mpPixelShader(nullptr)
	, mpDepthReadOnlyState(nullptr)
{

	Renderer& renderer = Renderer::GetInstance();

	mpMesh = renderer.GetMeshOrNull(TEXT("Sphere"));
	ASSERT(mpMesh != nullptr);

	mpVertexShader = renderer.GetVertexShaderOrNull(SHADER_PATH("VSBoundingSphere.hlsl"));
	ASSERT(mpVertexShader != nullptr);

	mpWireframeState = renderer.GetWireframeState();
	ASSERT(mpWireframeState != nullptr);

	mpPixelShader = renderer.GetPixelShaderOrNull(SHADER_PATH("PSBoundingSphere.hlsl"));
	ASSERT(mpPixelShader != nullptr);

	mpBlendState = renderer.GetBlendState();
	ASSERT(mpBlendState != nullptr);

	mpDepthReadOnlyState = renderer.GetDepthStencilState();
	ASSERT(mpDepthReadOnlyState != nullptr);
}

void DebugSphere::Draw(ID3D11DeviceContext& deviceContext)
{
	mpMesh->Bind(deviceContext);

	deviceContext.VSSetShader(mpVertexShader, nullptr, 0);
	deviceContext.PSSetShader(mpPixelShader, nullptr, 0);

	const Matrix scale = Matrix::CreateScale(mScale);
	const Matrix translation = Matrix::CreateTranslation(mCenter);

	const Matrix worldMat = scale * translation;

	Renderer& renderer = Renderer::GetInstance();
	renderer.UpdateCBWorldMatrix({ worldMat.Transpose() });

	constexpr UINT SAMPLE_MASK = 0xFFFFFFFF;
	deviceContext.OMSetBlendState(mpBlendState, nullptr, SAMPLE_MASK);
	deviceContext.OMSetDepthStencilState(mpDepthReadOnlyState, 0);
	deviceContext.RSSetState(mpWireframeState);
	{
		deviceContext.DrawIndexed(mpMesh->GetIndexCount(), 0, 0);
	}
	deviceContext.RSSetState(nullptr);
	deviceContext.OMSetDepthStencilState(nullptr, 0);
	deviceContext.OMSetBlendState(nullptr, nullptr, SAMPLE_MASK);
}
