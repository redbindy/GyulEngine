#include "Material.h"

#include "Core/Assert.h"
#include "TextureManager.h"
#include "Texture.h"
#include "ShaderManager.h"
#include "Renderer/Renderer.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"

Material::Material(
	const std::string& path,
	const std::string& texturePath,
	const std::string& vertexShaderPath,
	const std::string& pixelShaderPath,
	ComPtr<ID3D11Buffer>& pMaterialBufferGPU,
	const ERasterizerType rasterizerType,
	const ESamplerType samplerType,
	const EBlendStateType blendStateType,
	const EDepthStencilType depthStencilType
)
	: mMaterialData{}
	, mPath(path)
	, mTexturePath(texturePath)
	, mVertexShaderPath(vertexShaderPath)
	, mPixelShaderPath(pixelShaderPath)
	, mpMaterialBufferGPU(pMaterialBufferGPU)
	, mRasterizerType(rasterizerType)
	, mSamplerType(samplerType)
	, mBlendStateType(blendStateType)
	, mDepthStencilType(depthStencilType)
{
	ASSERT(pMaterialBufferGPU != nullptr);

	mMaterialData.bUseTexture = true;
	mMaterialData.diffuseColor = Vector3(1.f, 1.f, 1.f);
	mMaterialData.specularColor = Vector3(1.f, 1.f, 1.f);
}

void Material::Bind(ID3D11DeviceContext& deviceContext) const
{
	TextureManager& textureManager = TextureManager::GetInstance();

	Texture* const pTexture = textureManager.GetTextureOrNull(mTexturePath);

	if (pTexture != nullptr)
	{
		pTexture->Bind(deviceContext);
	}

	ShaderManager& shaderManager = ShaderManager::GetInstance();

	ID3D11VertexShader* const pVS = shaderManager.GetVertexShaderOrNull(mVertexShaderPath);
	ASSERT(pVS != nullptr);

	deviceContext.VSSetShader(pVS, nullptr, 0);

	ID3D11PixelShader* const pPS = shaderManager.GetPixelShaderOrNull(mPixelShaderPath);
	ASSERT(pPS != nullptr);

	deviceContext.PSSetShader(pPS, nullptr, 0);

	deviceContext.UpdateSubresource(
		mpMaterialBufferGPU.Get(),
		0,
		nullptr,
		&mMaterialData,
		0,
		0
	);

	deviceContext.PSSetConstantBuffers(
		Renderer::ConstantBufferSlot::CB_MATERIAL_SLOT,
		1,
		mpMaterialBufferGPU.GetAddressOf()
	);

	Renderer& renderer = Renderer::GetInstance();

	deviceContext.RSSetState(renderer.GetRasterizerState(mRasterizerType));

	ID3D11SamplerState* const samplerStates[] = {
		renderer.GetSamplerState(mSamplerType)
	};

	deviceContext.PSSetSamplers(0, 1, samplerStates);
	deviceContext.OMSetBlendState(renderer.GetBlendState(mBlendStateType), nullptr, 0xFFFFFFFF);
	deviceContext.OMSetDepthStencilState(renderer.GetDepthStencilState(mDepthStencilType), 0);
}

void Material::DrawEditorUI()
{
	ImGui::PushID(mPath.c_str());

	ImGui::SeparatorText(UTF8_TEXT("머터리얼"));

	ImGui::Checkbox(UTF8_TEXT("텍스처 사용"), &mMaterialData.bUseTexture);

	ImGui::SliderFloat3(UTF8_TEXT("Ambient"), reinterpret_cast<float*>(&mMaterialData.ambientColor), 0.f, 1.f, "%.2f");
	ImGui::SliderFloat3(UTF8_TEXT("Diffuse"), reinterpret_cast<float*>(&mMaterialData.diffuseColor), 0.f, 1.f, "%.2f");
	ImGui::SliderFloat3(UTF8_TEXT("Specular"), reinterpret_cast<float*>(&mMaterialData.specularColor), 0.f, 1.f, "%.2f");
	ImGui::SliderFloat(UTF8_TEXT("Shininess"), &mMaterialData.shininess, 1.f, 256.f, "%.1f");

	ImGui::Text("Path: %s", mPath.c_str());
	ImGui::Text("Texture: %s", mTexturePath.c_str());

	ImGui::Text("VertexShader: %s", mVertexShaderPath.c_str());
	ImGui::Text("PixelShader: %s", mPixelShaderPath.c_str());

	ImGui::Text("Rasterizer: %s", GetRasterizerTypeName(mRasterizerType));
	ImGui::Text("Sampler: %s", GetSamplerTypeName(mSamplerType));
	ImGui::Text("Blend State: %s", GetBlendTypeName(mBlendStateType));
	ImGui::Text("Depth Stencil: %s", GetDepthStencilTypeName(mDepthStencilType));

	ImGui::PopID();
}
