#pragma once

#include "Component.h"
#include "Core/MathHelper.h"

class CameraControllerComponent final : public Component
{
public:
	CameraControllerComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder = 10u);
	virtual ~CameraControllerComponent() = default;

	virtual void Update(const float deltaTime) override;

private:
	float mMoveSpeed;

private:
	CameraControllerComponent(const CameraControllerComponent& other) = delete;
	CameraControllerComponent& operator=(const CameraControllerComponent& other) = delete;
	CameraControllerComponent(CameraControllerComponent&& other) = delete;
	CameraControllerComponent& operator=(CameraControllerComponent&& other) = delete;
};