#pragma once

#include <string>

#include <d3d11.h>

#include "IUIDrawable.h"
#include "ETypeForRenderer.h"
#include "Renderer.h"

class Material : public IUIDrawable
{
public:
	Material(
		const std::string& texturePath,
		const ESamplerType eSamplerType,
		const std::string& vertexShaderPath,
		const std::string& pixelShaderPath
	);
	Material(const std::string& vertexShaderPath, const std::string& pixelShaderPath);
	Material();
	virtual ~Material();
	Material(const Material& other) = delete;
	Material& operator=(const Material& other) = delete;
	Material(Material&& other) = delete;
	Material& operator=(Material&& other) = delete;

	virtual void Bind(ID3D11DeviceContext& deviceContext) const;

	virtual void DrawUI() override;

private:
	struct CBMaterial
	{
		bool bUseTexture;
		uint8_t padding[GET_PADDING_SIZE_CBUFFER(bool)];
	};
	static_assert(sizeof(CBMaterial) % 16 == 0);

private:
	std::string mTexturePath;
	ID3D11ShaderResourceView* mpTextureViewGPU;

	bool mbOnTexture;
	ID3D11Buffer* mpCBMaterialGPU;

	ESamplerType mSamplerType;
	ID3D11SamplerState* mpSamplerState;

	std::string mVertexShaderPath;
	ID3D11VertexShader* mpVertexShader;

	std::string mPixelShaderPath;
	ID3D11PixelShader* mpPixelShader;
};