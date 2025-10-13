#include "Material.h"

#include "Renderer.h"
#include "StringHelper.h"

enum
{
	DEFAULT_BUFFER_SIZE = 256
};

Material::Material()
	: mTexturePath(TEXT("Default.dds"))
	, mpTextureViewGPU(nullptr)
	, mSamplerType(ESamplerType::WRAP_LINEAR)
	, mpSamplerState(nullptr)
	, mVertexShaderPath(TEXT("VSBasic.hlsl"))
	, mpVertexShader(nullptr)
	, mPixelShaderPath(TEXT("PSBasic.hlsl"))
	, mpPixelShader(nullptr)
{
	Renderer* const pRenderer = Renderer::GetInstance();
	ASSERT(pRenderer != nullptr);

	mpTextureViewGPU = pRenderer->GetTextureViewOrNull(mTexturePath.c_str());
	ASSERT(mpTextureViewGPU != nullptr);

	mpSamplerState = pRenderer->GetSamplerState(mSamplerType);
	ASSERT(mpSamplerState != nullptr);

	mpVertexShader = pRenderer->GetVertexShaderOrNull(mVertexShaderPath.c_str());
	ASSERT(mpVertexShader != nullptr);

	mpPixelShader = pRenderer->GetPixelShaderOrNull(mPixelShaderPath.c_str());
	ASSERT(mpPixelShader != nullptr);
}

void Material::Bind(ID3D11DeviceContext& deviceContext) const
{
	deviceContext.VSSetShader(mpVertexShader, nullptr, 0);
	deviceContext.PSSetShader(mpPixelShader, nullptr, 0);
	deviceContext.PSSetSamplers(0, 1, &mpSamplerState);
	deviceContext.PSSetShaderResources(0, 1, &mpTextureViewGPU);
}

void Material::DrawUI()
{
	if (ImGui::CollapsingHeader("Material"))
	{
		char buffer[DEFAULT_BUFFER_SIZE];
		ConvertWideToMulti(buffer, mTexturePath.c_str());

		ImGui::Text("Texture: %s", buffer);

		ImGui::Text("Sampler: %s", GetSamplerTypeString(mSamplerType));

		ConvertWideToMulti(buffer, mVertexShaderPath.c_str());
		ImGui::Text("VertexShader: %s", buffer);

		ConvertWideToMulti(buffer, mPixelShaderPath.c_str());
		ImGui::Text("PixelShader: %s", buffer);
	}
}
