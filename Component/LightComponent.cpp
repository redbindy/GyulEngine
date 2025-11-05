#include "LightComponent.h"

#include "ComponentGenerator.h"
#include "ComHelper.h"
#include "Actor.h"
#include "Renderer/Renderer.h"

LightComponent::LightComponent(Actor* const pOwner)
	: Component(pOwner, ComponentGenerator::GetNameString(ComponentGenerator::EComponentType::Light))
	, mLight(Renderer::GetInstance().AcquireLightOrNull())
{
	ASSERT(pOwner != nullptr);
}

LightComponent::~LightComponent()
{
	Renderer& renderer = Renderer::GetInstance();

	renderer.ReturnLight(mLight);
}

void LightComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	Actor& actor = *GetOwner();

	mLight->position = actor.GetPosition();
	mLight->direction = Vector3::Transform(Vector3::UnitZ, actor.GetRotation());
}

void LightComponent::DrawUI()
{
	Component::DrawUI();

	ImGui::SliderFloat3("Strength", reinterpret_cast<float*>(&mLight->strength), 0.f, 1.f, "%.2f");

	constexpr float FALL_OFF_MIN = 0.f;
	constexpr float FALL_OFF_MAX = 100.f;

	ImGui::SliderFloat("FallOffStart", &mLight->fallOffStart, FALL_OFF_MIN, FALL_OFF_MAX, "%.2f");
	ImGui::SliderFloat("FallOffEnd", &mLight->fallOffEnd, mLight->fallOffStart + 1.f, FALL_OFF_MAX + 0.001f, "%.2f");

	ImGui::SliderFloat("SpotPower", &mLight->spotPower, 0.f, 100.f, "%.2f");
}
