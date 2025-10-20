#include "InteractionCollider.h"

#include "Actor.h"
#include "GameCore.h"
#include "Renderer/Renderer.h"

InteractionCollider::InteractionCollider(Actor* const pOwner, const BoundingSphere& boundingSphereLocal)
	: mpOwner(pOwner)
	, mBoundingSphereLocal(boundingSphereLocal)
	, mMouseStartWorld()
	, mMouseEndWorld()
	, mMouseRay()
	, mCollisionDist(-1.f)
	, mbPicked(false)
	, mPrevRatio(1.f)
	, mPrevVector()
{

}

bool InteractionCollider::CheckCollision(const Vector2 mouseCoord, float& outDist)
{
	const Matrix transform = mpOwner->GetTransform();

	BoundingSphere boundingSphereWorld;
	mBoundingSphereLocal.Transform(boundingSphereWorld, transform);

	Renderer& renderer = Renderer::GetInstance();
	const Vector2 screenSize(static_cast<float>(renderer.GetWidth()), static_cast<float>(renderer.GetHeight()));

	Vector2 ndcXY = 2.f * mouseCoord / screenSize - Vector2(1.f, 1.f);
	ndcXY.y *= -1.f;

	const Vector3 ndcStart(ndcXY.x, ndcXY.y, 0.f);
	mMouseStartWorld = renderer.Unproject(ndcStart);

	const Vector3 ndcEnd(ndcXY.x, ndcXY.y, 1.f);
	mMouseEndWorld = renderer.Unproject(ndcEnd);

	Vector3 dir = mMouseEndWorld - mMouseStartWorld;
	dir.Normalize();

	mMouseRay = Ray(mMouseStartWorld, dir);

	const bool bRet = mMouseRay.Intersects(boundingSphereWorld, mCollisionDist);
	outDist = mCollisionDist;

	return bRet;
}

void InteractionCollider::OnCollision()
{
	Renderer& renderer = Renderer::GetInstance();

	const Matrix transform = mpOwner->GetTransform();

	BoundingSphere boundingSphereWorld;
	mBoundingSphereLocal.Transform(boundingSphereWorld, transform);

	renderer.SetDebugSphere(boundingSphereWorld.Center, boundingSphereWorld.Radius);

	GameCore& gameCore = GameCore::GetInstance();

	if (gameCore.IsKeyPressed(VK_LBUTTON))
	{
		const Vector3 pickPoint = mMouseRay.position + mMouseRay.direction * mCollisionDist;
		const Vector3 endToStart = mMouseEndWorld - mMouseStartWorld;

		if (mbPicked)
		{
			const Vector3 newPoint = mMouseStartWorld + mPrevRatio * endToStart;

			const Vector3 translation = newPoint - mPrevVector;

			mpOwner->SetPosition(mpOwner->GetPosition() + translation);
			mPrevVector = newPoint;
		}
		else
		{
			mPrevRatio = mCollisionDist / endToStart.Length();
			mPrevVector = pickPoint;
		}

		mbPicked = true;
	}
	else if (gameCore.IsKeyPressed(VK_RBUTTON))
	{
		const Vector3 pickPoint = mMouseRay.position + mMouseRay.direction * mCollisionDist;

		const Vector3 mCurrVector = pickPoint - boundingSphereWorld.Center;

		if (mbPicked)
		{
			const Vector3 prevToCurr = mCurrVector - mPrevVector;
			const float prevToCurrLength = prevToCurr.LengthSquared();

			if (prevToCurrLength > FLT_EPSILON)
			{
				const Quaternion q = Quaternion::FromToRotation(mPrevVector, mCurrVector);

				Actor& actor = *mpOwner;

				Quaternion rotation = Quaternion::Concatenate(q, actor.GetRotation());
				rotation.Normalize();

				actor.SetRotation(rotation);

				mPrevVector = mCurrVector;
			}
		}
		else
		{
			mPrevVector = mCurrVector;
		}

		mbPicked = true;
	}
	else
	{
		mbPicked = false;
	}
}
