#include "Material.h"

#include "Core/Assert.h"
#include "TextureManager.h"
#include "Texture.h"
#include "ShaderManager.h"
#include "Renderer/Renderer.h"

Material::Material(
	const std::string& path,
	const std::string& texturePath,
	const std::string& vertexShaderPath,
	const std::string& pixelShaderPath,
	ComPtr<ID3D11Buffer>& pMaterialBufferGPU
)
	: mTexturePaths(texturePath)
	, mVertexShaderPath(vertexShaderPath)
	, mPixelShaderPath(pixelShaderPath)
	, mpMaterialBufferGPU(pMaterialBufferGPU)
	, mRasterizerType(ERasterizerType::SOLID)
	, mSamplerType(ESamplerType::LINEAR_WRAP)
	, mBlendStateType(EBlendStateType::OPAQUE)
	, mDepthStencilType(EDepthStencilType::DEPTH_ENABLED)
{
	ASSERT(pMaterialBufferGPU != nullptr);
}

void Material::Bind(ID3D11DeviceContext& deviceContext) const
{
	TextureManager& textureManager = TextureManager::GetInstance();

	Texture* const pTexture = textureManager.GetTextureOrNull(mTexturePaths);
	ASSERT(pTexture != nullptr);

	pTexture->Bind(deviceContext);

	ShaderManager& shaderManager = ShaderManager::GetInstance();

	ID3D11VertexShader* const pVS = shaderManager.GetVertexShaderOrNull(mVertexShaderPath);
	ASSERT(pVS != nullptr);

	deviceContext.VSSetShader(pVS, nullptr, 0);

	ID3D11PixelShader* const pPS = shaderManager.GetPixelShaderOrNull(mPixelShaderPath);
	ASSERT(pPS != nullptr);

	deviceContext.PSSetShader(pPS, nullptr, 0);

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