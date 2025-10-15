#pragma once

#include "Component.h"

class CameraComponent final : public Component
{
public:
	CameraComponent(Actor* const pOwner);
	virtual ~CameraComponent();
	CameraComponent(const CameraComponent& other) = delete;
	CameraComponent& operator=(const CameraComponent& other) = delete;
	CameraComponent(CameraComponent&& other) = delete;
	CameraComponent& operator=(CameraComponent&& other) = delete;

	virtual void Update(const float deltaTime) override;
	virtual void DrawUI() override;

	inline void SetActive()
	{
		mbActive = true;
	}

private:
	bool mbActive;

	bool mbOrthogonal;

	// only orthogonal
	float mViewWidth;
	float mViewHeight;

	float mNearZ;
	float mFarZ;

	// only perspective
	float mFov;
};