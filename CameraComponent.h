#pragma once

#include "Component.h"

class CameraComponent final : public Component
{
public:
	CameraComponent(Actor* const pOwner);
	virtual ~CameraComponent() = default;
	CameraComponent(const CameraComponent& other) = delete;
	CameraComponent& operator=(const CameraComponent& other) = delete;
	CameraComponent(CameraComponent&& other) = delete;
	CameraComponent& operator=(CameraComponent&& other) = delete;

	virtual void Update(const float deltaTime) override;
	virtual void DrawUI() override;

private:
	bool mbMain;
	bool mbOrthogonoal;

	float mNearPlane;
	float mFarPlane;
	float mFov;
};