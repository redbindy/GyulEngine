#include "SpriteComponent.h"

#include "ComponentGenerator.h"
#include "Renderer/Renderer.h"

SpriteComponent::SpriteComponent(Actor* const pOwner)
	: MeshComponent(
		pOwner,
		ComponentGenerator::GetNameString(ComponentGenerator::EComponentType::SPRITE),
		Renderer::GetInstance().GetMeshOrNull("Square"),
		Renderer::GetInstance().GetMaterialOrNull("Sprite")
	)
{
	ASSERT(pOwner != nullptr);
}

void SpriteComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);
}

void SpriteComponent::Draw(ID3D11DeviceContext& deviceContext) const
{
	Renderer& renderer = Renderer::GetInstance();

	constexpr UINT SAMPLE_MASK = 0xFFFFFFFF;

	deviceContext.RSSetState(renderer.GetNonCullingState());
	deviceContext.OMSetBlendState(renderer.GetBlendState(), nullptr, SAMPLE_MASK);
	{
		MeshComponent::Draw(deviceContext);
	}
	deviceContext.OMSetBlendState(nullptr, nullptr, SAMPLE_MASK);
	deviceContext.RSSetState(nullptr);
}
