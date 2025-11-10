#pragma once

#include "Component.h"

class CameraComponent final : public Component
{
public:
	CameraComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder = 10u);
	virtual ~CameraComponent() = default;

	virtual void Update(const float deltaTime) override;

private:

	// orthographic
	bool mbOrhographic;

	float mViewWidth;
	float mViewHeight;

	float mNearZ;
	float mFarZ;

	// perspective
	float mFov;

private:
	CameraComponent(const CameraComponent& other) = delete;
	CameraComponent& operator=(const CameraComponent& other) = delete;
	CameraComponent(CameraComponent&& other) = delete;
	CameraComponent& operator=(CameraComponent&& other) = delete;
};