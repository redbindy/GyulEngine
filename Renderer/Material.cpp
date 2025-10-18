#include "Material.h"

#include "CommonDefs.h"
#include "Renderer.h"
#include "StringHelper.h"

enum
{
	DEFAULT_BUFFER_SIZE = 256
};

Material::Material()
	: mTexturePath(RESOURCE_PATH("Default.dds"))
	, mpTextureViewGPU(nullptr)
	, mSamplerType(ESamplerType::WRAP_LINEAR)
	, mpSamplerState(nullptr)
	, mVertexShaderPath(SHADER_PATH("VSBasic.hlsl"))
	, mpVertexShader(nullptr)
	, mPixelShaderPath(SHADER_PATH("PSBasic.hlsl"))
	, mpPixelShader(nullptr)
{
	Renderer& renderer = Renderer::GetInstance();

	mpTextureViewGPU = renderer.GetTextureViewOrNull(mTexturePath.c_str());
	ASSERT(mpTextureViewGPU != nullptr);

	mpSamplerState = renderer.GetSamplerState(mSamplerType);
	ASSERT(mpSamplerState != nullptr);

	mpVertexShader = renderer.GetVertexShaderOrNull(mVertexShaderPath.c_str());
	ASSERT(mpVertexShader != nullptr);

	mpPixelShader = renderer.GetPixelShaderOrNull(mPixelShaderPath.c_str());
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
	if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		char buffer[DEFAULT_BUFFER_SIZE];
		ConvertWideToMulti(buffer, mTexturePath.c_str());

		ImGui::Text("Texture: %s", buffer);

		ImGui::Text("Sampler: %s", GetSamplerTypeString(mSamplerType));

		ConvertWideToMulti(buffer, mVertexShaderPath.c_str());
		ImGui::Text("VertexShader: %s", buffer);

		ConvertWideToMulti(buffer, mPixelShaderPath.c_str());
		ImGui::Text("PixelShader: %s", buffer);

		ImGui::TreePop();
	}
}
