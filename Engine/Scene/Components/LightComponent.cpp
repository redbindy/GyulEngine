#include "LightComponent.h"

#include "Renderer/Renderer.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"
#include "../Actor.h"

LightComponent::LightComponent(
	Actor* const pOwner,
	const char* const label,
	const uint32_t updateOrder
)
	: Component(pOwner, label, updateOrder)
	, mLight
	{
		Vector3::Zero, // position
		1.f,           // falloffStart
		Vector3(1.f, 1.f, 1.f), // strength
		10.f,          // falloffEnd
		Vector3::UnitZ, // direction
		64.f           // spotPower
	}
{

}

void LightComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	Actor& owner = GetOwner();

	mLight.position = owner.GetPosition();
	mLight.direction = Vector3::Transform(Vector3::UnitZ, owner.GetRotation());

	Renderer& renderer = Renderer::GetInstance();
	renderer.EnqueueLight(mLight);
}

void LightComponent::DrawEditorUI()
{
	if (ImGui::TreeNodeEx(GetLabel(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		constexpr float FALL_OFF_MIN = 0.f;
		constexpr float FALL_OFF_MAX = 1000.f;

		ImGui::SliderFloat3(UTF8_TEXT("°­µµ"), reinterpret_cast<float*>(&mLight.strength), 0.f, 1.f, "%.2f");
		ImGui::SliderFloat(UTF8_TEXT("°¨¼è ½ÃÀÛ"), &mLight.falloffStart, FALL_OFF_MIN, FALL_OFF_MAX, "%.2f");
		ImGui::SliderFloat(UTF8_TEXT("°¨¼è ³¡"), &mLight.falloffEnd, mLight.falloffStart + 1.f, FALL_OFF_MAX + 0.001f, "%.2f");
		ImGui::SliderFloat(UTF8_TEXT("½ºÆÌ ÆÄ¿ö"), &mLight.spotPower, 0.f, 128.f);

		ImGui::TreePop();
	}
}
