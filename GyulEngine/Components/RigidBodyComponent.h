#pragma once

#include "../Core/MathHelper.h"

#include "Component.h"

class RigidBodyComponent : public Component
{
public:
	RigidBodyComponent();
	virtual ~RigidBodyComponent() = default;

	void Initialize() override;
	void Update() override;
	void LateUpdate() override;
	void Render() const override;

	void SetGround(const bool bGround)
	{
		mbGround = bGround;
	}

	void SetVelocity(const Vector2 velocity)
	{
		mVelocity = velocity;
	}

	Vector2 GetVelocity() const
	{
		return mVelocity;
	}

private:
	bool mbGround;

	float mMass;
	float mFriction;

	Vector2 mForce;
	Vector2 mAcceleration;
	Vector2 mVelocity;
	Vector2 mLimitedVelocity;
	Vector2 mGravity;

private:
	RigidBodyComponent(const RigidBodyComponent& other) = delete;
	RigidBodyComponent& operator=(const RigidBodyComponent& other) = delete;
	RigidBodyComponent(RigidBodyComponent&& other) noexcept = delete;
	RigidBodyComponent& operator=(RigidBodyComponent&& other) noexcept = delete;
};