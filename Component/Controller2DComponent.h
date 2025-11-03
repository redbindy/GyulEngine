#pragma once

#include "Component.h"

class Controller2DComponent final : public Component
{
public:
	Controller2DComponent(Actor* const pOwner);
	virtual ~Controller2DComponent() = default;
	Controller2DComponent(const Controller2DComponent& other) = delete;
	Controller2DComponent& operator=(const Controller2DComponent& other) = delete;
	Controller2DComponent(Controller2DComponent&& other) = delete;
	Controller2DComponent& operator=(Controller2DComponent&& other) = delete;

	virtual void Update(const float deltaTime) override;

private:
};