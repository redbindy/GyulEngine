#pragma once

#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Actor;

class InteractionCollider final
{
public:
	InteractionCollider(Actor* const pOwner, const BoundingSphere& boundingSphereLocal);
	~InteractionCollider() = default;
	InteractionCollider(const InteractionCollider& other) = delete;
	InteractionCollider& operator=(const InteractionCollider& other) = delete;
	InteractionCollider(InteractionCollider&& other) = default;
	InteractionCollider& operator=(InteractionCollider&& other) = default;

	bool CheckCollision(const Vector2 mouseCoord, float& outDist);
	void OnCollision();

	const Actor* GetOwner() const
	{
		return mpOwner;
	}

private:
	Actor* mpOwner;
	BoundingSphere mBoundingSphereLocal;

	Vector3 mMouseStartWorld;
	Vector3 mMouseEndWorld;
	Ray mMouseRay;
	float mCollisionDist;

	bool mbPicked;
	float mPrevRatio;
	Vector3 mPrevVector;
};