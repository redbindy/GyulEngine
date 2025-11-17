#pragma once

#include "Component.h"
#include "Renderer/Light.h"

class LightComponent final : public Component
{
public:
	LightComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder = 10u);
	virtual ~LightComponent() = default;

	virtual void Update(const float deltaTime) override;
	virtual void DrawEditorUI() override;

private:
	Light mLight;

private:
	LightComponent(const LightComponent& other) = delete;
	LightComponent& operator=(const LightComponent& other) = delete;
	LightComponent(LightComponent&& other) = delete;
	LightComponent& operator=(LightComponent&& other) = delete;
};