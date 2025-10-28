#pragma once

#include "Material.h"

#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class ClassicLightingMaterial final : public Material
{
public:
	ClassicLightingMaterial();
	~ClassicLightingMaterial();
	ClassicLightingMaterial(const ClassicLightingMaterial& other) = delete;
	ClassicLightingMaterial& operator=(const ClassicLightingMaterial& other) = delete;
	ClassicLightingMaterial(ClassicLightingMaterial&& other) = delete;
	ClassicLightingMaterial& operator=(ClassicLightingMaterial&& other) = delete;

	virtual void Bind(ID3D11DeviceContext& deviceContext) const override;

	virtual void DrawUI() override;

private:
	struct CBClassicLightingMaterial
	{
		Vector3 ambient;
		float shininess;

		Vector3 diffuse;
		uint8_t paddin1[GET_PADDING_SIZE_CBUFFER(Vector3)];

		Vector3 specular;
		uint8_t padding2[GET_PADDING_SIZE_CBUFFER(Vector3)];
	};
	static_assert(sizeof(CBClassicLightingMaterial) % 16 == 0);

private:
	CBClassicLightingMaterial mCBClassicLightingMaterial;
	ID3D11Buffer* mpClassicLightingMaterialGPU;
};