#pragma once

#include <string>

#include <d3d11.h>

#include "IUIDrawable.h"
#include "ETypeForRenderer.h"

class Material final : public IUIDrawable
{
public:
	Material();
	~Material() = default;
	Material(const Material& other) = delete;
	Material& operator=(const Material& other) = delete;
	Material(Material&& other) = delete;
	Material& operator=(Material&& other) = delete;

	void Bind(ID3D11DeviceContext& deviceContext) const;

	virtual void DrawUI() override;

private:
	std::wstring mTexturePath;
	ID3D11ShaderResourceView* mpTextureViewGPU;

	ESamplerType mSamplerType;
	ID3D11SamplerState* mpSamplerState;

	std::wstring mVertexShaderPath;
	ID3D11VertexShader* mpVertexShader;

	std::wstring mPixelShaderPath;
	ID3D11PixelShader* mpPixelShader;
};