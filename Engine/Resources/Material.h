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
	// 구조체에 대해서 초기화 경고 끄기
#pragma warning(push)
#pragma warning(disable : 26495)
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
#pragma warning(pop)

public:
	Material(
		const std::string& path,
		const std::string& texturePath,
		const std::string& vertexShaderPath,
		const std::string& pixelShaderPath,
		ComPtr<ID3D11Buffer>& pMaterialBufferGPU,
		const ERasterizerType rasterizerType,
		const ESamplerType samplerType,
		const EBlendStateType blendStateType,
		const EDepthStencilType depthStencilType
	);
	~Material() = default;

	void Bind(ID3D11DeviceContext& deviceContext) const;

	virtual void DrawEditorUI() override;

	void SetTexturePath(const std::string& texturePath)
	{
		mTexturePath = texturePath;
	}

	void SetVertexShaderPath(const std::string& vertexShaderPath)
	{
		mVertexShaderPath = vertexShaderPath;
	}

	void SetPixelShaderPath(const std::string& pixelShaderPath)
	{
		mPixelShaderPath = pixelShaderPath;
	}

	void SetRasterizerType(const ERasterizerType rasterizerType)
	{
		mRasterizerType = rasterizerType;
	}

	void SetSamplerType(const ESamplerType samplerType)
	{
		mSamplerType = samplerType;
	}

	void SetBlendStateType(const EBlendStateType blendStateType)
	{
		mBlendStateType = blendStateType;
	}

	void SetDepthStencilType(const EDepthStencilType depthStencilType)
	{
		mDepthStencilType = depthStencilType;
	}

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