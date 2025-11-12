#include "InteractionSystem.h"

#include "Renderer/Renderer.h"
#include "Scene/Actor.h"
#include "InputSystem.h"

enum
{
	DEFAULT_BUFFER_SIZE = 32,
};

InteractionSystem* InteractionSystem::spInstance = nullptr;

InteractionSystem::InteractionSystem()
	: mMouseStartWorld{ 0.0f, 0.0f, 0.0f }
	, mMouseEndWorld{ 0.0f, 0.0f, 0.0f }
	, mMouseRay{}
	, mCollisionDist{ 0.0f, }
	, mbPicked{ false, }
	, mPrevRatio{ 0.0f }
	, mPrevVector{ 0.0f, 0.0f, 0.0f }
	, mPickedCollider{ nullptr, }
{

}

void InteractionSystem::Pick(const std::string& scene)
{
	ASSERT(mCollidersMap.find(scene) != mCollidersMap.end());

	updateInteractionInfo();

	mCollisionDist = 10000.f;
	mPickedCollider = { nullptr, };

	for (const InteractionCollider& collider : mCollidersMap[scene])
	{
		BoundingSphere boundingSphereWorld;

		collider.boundingSphereLocal.Transform(
			boundingSphereWorld,
			collider.pActorOrNull->GetTransform()
		);

		float dist = 0.f;
		if (mMouseRay.Intersects(boundingSphereWorld, dist))
		{
			if (dist < mCollisionDist)
			{
				mCollisionDist = dist;
				mPickedCollider = collider;
			}
		}
	}

	if (mPickedCollider.pActorOrNull != nullptr)
	{
		InputSystem& inputSystem = InputSystem::GetInstance();

		if (inputSystem.IsKeyPressed(VK_LBUTTON))
		{
			const Vector3 pickPoint = mMouseRay.position + mMouseRay.direction * mCollisionDist;
			const Vector3 endToStart = mMouseEndWorld - mMouseStartWorld;

			mPrevRatio = mCollisionDist / endToStart.Length();

			mPrevVector = pickPoint;

			mbPicked = true;
		}
		else if (inputSystem.IsKeyPressed(VK_RBUTTON))
		{
			const Vector3 pickPoint = mMouseRay.position + mMouseRay.direction * mCollisionDist;
			const Vector3 endToStart = mMouseEndWorld - mMouseStartWorld;

			mPrevRatio = mCollisionDist / endToStart.Length();

			mPrevVector = pickPoint - mPickedCollider.pActorOrNull->GetPosition();

			mbPicked = true;
		}
	}
}

void InteractionSystem::ReleasePick()
{
	mPickedCollider = { nullptr, };
	mbPicked = false;
}

void InteractionSystem::Update()
{
	if (mPickedCollider.pActorOrNull == nullptr)
	{
		return;
	}

	updateInteractionInfo();

	InputSystem& inputSystem = InputSystem::GetInstance();

	Actor& pickedActor = *mPickedCollider.pActorOrNull;

	BoundingSphere boundingSphereWorld;
	mPickedCollider.boundingSphereLocal.Transform(
		boundingSphereWorld,
		pickedActor.GetTransform()
	);

	mMouseRay.Intersects(boundingSphereWorld, mCollisionDist);

	if (inputSystem.IsKeyPressed(VK_LBUTTON))
	{
		const Vector3 pickPoint = mMouseRay.position + mMouseRay.direction * mCollisionDist;
		const Vector3 endToStart = mMouseEndWorld - mMouseStartWorld;

		const Vector3 newPoint = mMouseStartWorld + mPrevRatio * endToStart;

		const Vector3 translation = newPoint - mPrevVector;

		pickedActor.SetPosition(pickedActor.GetPosition() + translation);
		mPrevVector = newPoint;
	}
	else if (inputSystem.IsKeyPressed(VK_RBUTTON))
	{
		const Vector3 pickPoint = mMouseRay.position + mMouseRay.direction * mCollisionDist;
		const Vector3 endToStart = mMouseEndWorld - mMouseStartWorld;

		const Vector3 newPoint = mMouseStartWorld + mPrevRatio * endToStart;

		const Vector3 mCurrVector = newPoint - pickedActor.GetPosition();

		const Vector3 prevToCurr = mCurrVector - mPrevVector;
		const float prevToCurrLength = prevToCurr.LengthSquared();

		if (prevToCurrLength > FLT_EPSILON)
		{
			const Quaternion q = Quaternion::FromToRotation(mPrevVector, mCurrVector);

			Quaternion rotation = Quaternion::Concatenate(q, pickedActor.GetRotation());
			rotation.Normalize();

			pickedActor.SetRotation(rotation);

			mPrevVector = mCurrVector;
		}
	}
}

void InteractionSystem::MakeSceneBuffer(const std::string& sceneName)
{
	std::vector<InteractionCollider> vector;
	vector.reserve(DEFAULT_BUFFER_SIZE);

	mCollidersMap.insert({ sceneName, std::vector(vector) });
}

void InteractionSystem::RemoveSceneBuffer(const std::string& sceneName)
{
	ASSERT(mCollidersMap.find(sceneName) != mCollidersMap.end());

	mCollidersMap.erase(sceneName);
}

void InteractionSystem::RegisterCollider(
	const std::string& scene,
	Actor* const pActor,
	const BoundingSphere& localSphere
)
{
	ASSERT(pActor != nullptr);

	mCollidersMap[scene].push_back({ pActor, localSphere });
}

void InteractionSystem::UnregisterCollider(const std::string& scene, Actor* const pActor)
{
	ASSERT(pActor != nullptr);

#define VECTOR_ITER std::vector<InteractionCollider>::iterator

	std::vector<InteractionCollider>& v = mCollidersMap[scene];

	for (VECTOR_ITER iter = v.begin(); iter != v.end(); ++iter)
	{
		if (iter->pActorOrNull == pActor)
		{
			v.erase(iter);

			break;
		}
	}

#undef VECTOR_ITER
}

void InteractionSystem::updateInteractionInfo()
{
	Renderer& renderer = Renderer::GetInstance();

	const D3D11_VIEWPORT& viewport = renderer.GetViewport();

	const Vector2 screenSize(viewport.Width, viewport.Height);

	InputSystem& inputSystem = InputSystem::GetInstance();
	const Vector2 mouseCoord = inputSystem.GetMousePosition();

	Vector2 ndcXY = 2.f * mouseCoord / screenSize - Vector2(1.f, 1.f);
	ndcXY.y = -ndcXY.y;

	const Vector3 ndcStart = Vector3(ndcXY.x, ndcXY.y, 0.f);
	mMouseStartWorld = renderer.Unproject(ndcStart);

	const Vector3 ndcEnd = Vector3(ndcXY.x, ndcXY.y, 1.f);
	mMouseEndWorld = renderer.Unproject(ndcEnd);

	Vector3 dir = mMouseEndWorld - mMouseStartWorld;
	dir.Normalize();

	mMouseRay = Ray(mMouseStartWorld, dir);
}

void InteractionSystem::Initialize()
{
	ASSERT(spInstance == nullptr);
	spInstance = new InteractionSystem();
}