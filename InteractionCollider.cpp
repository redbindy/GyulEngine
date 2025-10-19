#include "InteractionCollider.h"

#include "Actor.h"
#include "GameCore.h"
#include "Renderer/Renderer.h"

InteractionCollider::InteractionCollider(Actor* const pOwner, const BoundingSphere& boundingSphereLocal)
	: mpOwner(pOwner)
	, mBoundingSphereLocal(boundingSphereLocal)
	, mbPicked(false)
	, mLastMousePosition()
{

}

bool InteractionCollider::CheckCollision(const Ray& ray, float& outDist) const
{
	const Matrix transform = mpOwner->GetTransform();

	BoundingSphere boundingSphereWorld;
	mBoundingSphereLocal.Transform(boundingSphereWorld, transform);

	return ray.Intersects(boundingSphereWorld, outDist);
}

void InteractionCollider::OnCollision()
{
	Renderer& renderer = Renderer::GetInstance();

	const Matrix transform = mpOwner->GetTransform();

	BoundingSphere boundingSphereWorld;
	mBoundingSphereLocal.Transform(boundingSphereWorld, transform);

	renderer.SetDebugSphere(boundingSphereWorld.Center, boundingSphereWorld.Radius);
}
