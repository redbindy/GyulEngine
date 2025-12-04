#include "RigidBodyComponent.h"

#include "../Core/Time.h"
#include "TransformComponent.h"
#include "../Scene/Actor.h"

RigidBodyComponent::RigidBodyComponent()
	: Component(L"RigidBodyComponent")
	, mbGround(false)
	, mMass(1.f)
	, mFriction(10.f)
	, mForce(Vector2::Zero)
	, mAcceleration(Vector2::Zero)
	, mVelocity(Vector2::Zero)
	, mLimitedVelocity(Vector2::Zero)
	, mGravity(Vector2::Zero)
{
}

void RigidBodyComponent::Initialize()
{

}

void RigidBodyComponent::Update()
{
	mAcceleration = mForce / mMass;

	Time& time = Time::GetInstance();

	mVelocity += mAcceleration * time.GetDeltaTime();

	if (mbGround)
	{
		Vector2 gravity = mGravity;
		gravity.Normalize();

		const float dot = mVelocity.Dot(gravity);

		mVelocity -= gravity * dot;
	}
	else
	{
		mVelocity += mGravity * time.GetDeltaTime();
	}

	Vector2 gravity = mGravity;
	gravity.Normalize();

	const float dot = mVelocity.Dot(gravity);
	gravity = gravity * dot;

	Vector2 sideVelocity = mVelocity - gravity;
	if (mLimitedVelocity.y < gravity.Length())
	{
		gravity.Normalize();
		gravity *= mLimitedVelocity.y;
	}

	if (mLimitedVelocity.x < sideVelocity.Length())
	{
		sideVelocity.Normalize();
		sideVelocity *= mLimitedVelocity.x;
	}
	mVelocity = gravity + sideVelocity;

	if (mVelocity != Vector2::Zero)
	{
		Vector2 friction = -mVelocity;
		friction.Normalize();

		friction = friction * mFriction * mMass * time.GetDeltaTime();

		if (mVelocity.LengthSquared() <= friction.LengthSquared())
		{
			mVelocity = Vector2::Zero;
		}
		else
		{
			mVelocity += friction;
		}
	}

	Actor* const pOwner = GetOwner();

	TransformComponent* pTransfrom = pOwner->GetComponent<TransformComponent>();

	Vector2 pos = pTransfrom->GetPosition();
	pos += mVelocity * time.GetDeltaTime();
	pTransfrom->SetPosition(pos);

	mForce = Vector2::Zero;
}

void RigidBodyComponent::LateUpdate()
{
}

void RigidBodyComponent::Render() const
{
}
