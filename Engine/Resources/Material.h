#pragma once

#include <string>
#include <vector>
#include <d3d11.h>

#include "Core/ComHelper.h"
#include "Core/MathHelper.h"
#include "Renderer/PipelineStateType.h"
#include "UI/IEditorUIDrawable.h"

class Texture;

class Material final : public IEditorUIDrawable
{
public:
	struct CBMaterial
	{
		Vector3 ambientColor;
		float shininess;
		Vector3 diffuseColor;
		float dummy0;
		Vector3 specularColor;
		float dummy1;

		bool bUseTexture;
		uint8_t padding[16 - sizeof(bool)];
	};
	static_assert(sizeof(CBMaterial) % 16 == 0);

public:
	Material(
		const std::string& path,
		const std::string& texturePath,
		const std::string& vertexShaderPath,
		const std::string& pixelShaderPath,
		ComPtr<ID3D11Buffer>& pMaterialBufferGPU
	);
	~Material() = default;

	void Bind(ID3D11DeviceContext& deviceContext) const;

	virtual void DrawEditorUI() override;

private:
	CBMaterial mMaterialData;

	std::string mPath;

	std::string mTexturePath;

	ComPtr<ID3D11Buffer> mpMaterialBufferGPU;

	std::string mVertexShaderPath;
	std::string mPixelShaderPath;

	ERasterizerType mRasterizerType;
	ESamplerType mSamplerType;
	EBlendStateType mBlendStateType;
	EDepthStencilType mDepthStencilType;

private:
	Material(const Material& other) = delete;
	Material& operator=(const Material& other) = delete;
	Material(Material&& other) = delete;
	Material& operator=(Material&& other) = delete;
};