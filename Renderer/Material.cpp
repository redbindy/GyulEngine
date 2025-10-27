#include "Material.h"

#include "CommonDefs.h"
#include "ComHelper.h"
#include "Renderer.h"
#include "StringHelper.h"

enum
{
	DEFAULT_BUFFER_SIZE = 256
};

Material::Material(
	const std::string& texturePath,
	const ESamplerType eSamplerType,
	const std::string& vertexShaderPath,
	const std::string& pixelShaderPath
)
	: mTexturePath(texturePath)
	, mpTextureViewGPU(nullptr)
	, mbOnTexture(true)
	, mpCBMaterialGPU(nullptr)
	, mSamplerType(eSamplerType)
	, mpSamplerState(nullptr)
	, mVertexShaderPath(vertexShaderPath)
	, mpVertexShader(nullptr)
	, mPixelShaderPath(pixelShaderPath)
	, mpPixelShader(nullptr)
{
	Renderer& renderer = Renderer::GetInstance();

	mpTextureViewGPU = renderer.GetTextureViewOrNull(mTexturePath);
	ASSERT(mpTextureViewGPU != nullptr);

	mpSamplerState = renderer.GetSamplerState(mSamplerType);
	ASSERT(mpSamplerState != nullptr);

	mpVertexShader = renderer.GetVertexShaderOrNull(mVertexShaderPath);
	ASSERT(mpVertexShader != nullptr);

	mpPixelShader = renderer.GetPixelShaderOrNull(mPixelShaderPath);
	ASSERT(mpPixelShader != nullptr);

	CBMaterial cb = { mbOnTexture };
	const bool bResult = renderer.TryCreateBuffer(EBufferType::CONSTANT, &cb, sizeof(cb), 0, mpCBMaterialGPU);
	ASSERT(bResult);
}

Material::Material(const std::string& vertexShaderPath, const std::string& pixelShaderPath)
	: Material(RESOURCE_PATH("Default.dds"), ESamplerType::WRAP_LINEAR, vertexShaderPath, pixelShaderPath)
{

}

Material::Material()
	: Material(RESOURCE_PATH("Default.dds"), ESamplerType::WRAP_LINEAR, SHADER_PATH("VSBasic.hlsl"), SHADER_PATH("PSBasic.hlsl"))
{

}

Material::~Material()
{
	SafeRelease(mpCBMaterialGPU);
}

void Material::Bind(ID3D11DeviceContext& deviceContext) const
{
	deviceContext.VSSetShader(mpVertexShader, nullptr, 0);
	deviceContext.PSSetShader(mpPixelShader, nullptr, 0);
	deviceContext.PSSetSamplers(0, 1, &mpSamplerState);
	deviceContext.PSSetShaderResources(0, 1, &mpTextureViewGPU);

	const CBMaterial cb = { mbOnTexture };
	deviceContext.UpdateSubresource(mpCBMaterialGPU, 0, nullptr, &cb, 0, 0);
	deviceContext.PSSetConstantBuffers(SlotNumber::MATERIAL, 1, &mpCBMaterialGPU);
}

void Material::DrawUI()
{
	ImGui::SeparatorText("Material");

	ImGui::Checkbox("Use Texture", &mbOnTexture);

	if (mbOnTexture)
	{
		ImGui::Text("Texture: %s", mTexturePath.c_str());
	}
	else
	{
		ImGui::TextDisabled("Texture: %s", mTexturePath.c_str());
	}

	ImGui::Text("Sampler: %s", GetSamplerTypeString(mSamplerType));

	ImGui::Text("VertexShader: %s", mVertexShaderPath.c_str());

	ImGui::Text("PixelShader: %s", mPixelShaderPath.c_str());
}
