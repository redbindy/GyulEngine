#include "ClassicLightingMaterial.h"

#include "ComHelper.h"

ClassicLightingMaterial::ClassicLightingMaterial()
	: Material(SHADER_PATH("VSBlinPhong.hlsl"), SHADER_PATH("PSBlinPhong.hlsl"))
	, mCBClassicLightingMaterial{}
	, mpClassicLightingMaterialGPU(nullptr)
{
	Renderer& renderer = Renderer::GetInstance();

	const bool bResult = renderer.TryCreateBuffer(
		EBufferType::CONSTANT,
		&mCBClassicLightingMaterial,
		sizeof(CBClassicLightingMaterial),
		0,
		mpClassicLightingMaterialGPU
	);
	ASSERT(bResult);
}

ClassicLightingMaterial::~ClassicLightingMaterial()
{
	SafeRelease(mpClassicLightingMaterialGPU);
}

void ClassicLightingMaterial::Bind(ID3D11DeviceContext& deviceContext) const
{
	Material::Bind(deviceContext);

	deviceContext.UpdateSubresource(mpClassicLightingMaterialGPU, 0, nullptr, &mCBClassicLightingMaterial, 0, 0);
	deviceContext.PSSetConstantBuffers(SlotNumber::MATERIAL_ADDITIONAL, 1, &mpClassicLightingMaterialGPU);
}

void ClassicLightingMaterial::DrawUI()
{
	Material::DrawUI();

	ImGui::SliderFloat3("Ambient", reinterpret_cast<float*>(&mCBClassicLightingMaterial.ambient), 0.f, 1.f, "%.2f");
	ImGui::SliderFloat3("Diffuse", reinterpret_cast<float*>(&mCBClassicLightingMaterial.diffuse), 0.f, 1.f, "%.2f");
	ImGui::SliderFloat3("Specular", reinterpret_cast<float*>(&mCBClassicLightingMaterial.specular), 0.f, 1.f, "%.2f");
	ImGui::SliderFloat("Shininess", &mCBClassicLightingMaterial.shininess, 0.f, 100.f, "%.2f");
}
