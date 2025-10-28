#pragma once

#include <d3d11.h>

#include "Component.h"

#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

struct Light;

class LightComponent final : public Component
{
public:
	LightComponent(Actor* const pOwner);
	~LightComponent();
	LightComponent(const LightComponent& other) = delete;
	LightComponent& operator=(const LightComponent& other) = delete;
	LightComponent(LightComponent&& other) = delete;
	LightComponent& operator=(LightComponent&& other) = delete;

	virtual void Update(const float deltaTime) override;

	virtual void DrawUI() override;

private:
	Light* mLight;
};