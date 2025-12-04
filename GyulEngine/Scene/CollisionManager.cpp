#include "CollisionManager.h"

#include "SceneManager.h"
#include "Scene.h"
#include "Layer.h"
#include "Actor.h"
#include "../Components/Collider2DComponent.h"
#include "../Components/TransformComponent.h"

enum
{
	DEFAULT_BUFFER_SIZE = 64
};

CollisionManager* CollisionManager::spInstance = nullptr;

CollisionManager::CollisionManager()
	: mCollisionMatrix{}
	, mCollisionMap()
{
	mCollisionMap.reserve(DEFAULT_BUFFER_SIZE);
}

void CollisionManager::Initialize()
{

}

void CollisionManager::Update()
{
	SceneManager& sceneManager = SceneManager::GetInstance();

	Scene* const pActiveScene = sceneManager.GetActiveScene();


}

void CollisionManager::LateUpdate()
{

}

void CollisionManager::Render() const
{

}

void CollisionManager::MaskCollsionLayer(
	const ELayerType left,
	const ELayerType right,
	const bool bEnable
)
{
	int row = 0;
	int col = 0;

	if (left <= right)
	{
		row = static_cast<int>(left);
		col = static_cast<int>(right);
	}
	else
	{
		row = static_cast<int>(right);
		col = static_cast<int>(left);
	}

	mCollisionMatrix[row][col] = bEnable;
}

void CollisionManager::CheckLayerCollision(
	Scene* const pScene,
	const ELayerType left,
	const ELayerType right
)
{
	const Layer* pLeftLayer = pScene->GetLayer(left);
	const std::vector<Actor*>& leftActors = pLeftLayer->GetActors();

	const Layer* pRightLayer = pScene->GetLayer(right);
	const std::vector<Actor*>& rightActors = pRightLayer->GetActors();

	for (Actor* const pLeftActor : leftActors)
	{
		if (pLeftActor->GetState() != Actor::EState::ACTIVE)
		{
			continue;
		}

		Collider2DComponent* const pLeftCollider = pLeftActor->GetComponent<Collider2DComponent>();
		if (pLeftCollider == nullptr)
		{
			continue;
		}

		for (Actor* const pRightActor : rightActors)
		{
			if (pRightActor->GetState() != Actor::EState::ACTIVE
				|| pLeftActor == pRightActor)
			{
				continue;
			}

			Collider2DComponent* const pRightCollider = pRightActor->GetComponent<Collider2DComponent>();
			if (pRightCollider == nullptr)
			{
				continue;
			}

			CollisionID id;
			id.left = pLeftCollider->GetID();
			id.right = pRightCollider->GetID();

			auto iter = mCollisionMap.find(id.id);
			if (iter == mCollisionMap.end())
			{
				mCollisionMap.insert({ id.id, false });

				iter = mCollisionMap.find(id.id);
			}


		}
	}
}

void CollisionManager::CreateInstance()
{
	ASSERT(spInstance == nullptr);

	spInstance = new CollisionManager();
}

bool CollisionManager::intersect(Collider2DComponent* pLeft, Collider2DComponent* pRight)
{
	Actor* pLeftActor = pLeft->GetOwner();
	Actor* pRightActor = pRight->GetOwner();

	TransformComponent* pLeftTrans = pLeftActor->GetComponent<TransformComponent>();
	TransformComponent* pRightTrans = pLeftActor->GetComponent<TransformComponent>();

	const Vector2 leftPos = pLeftTrans->GetPosition() + pLeft->GetOffset();;
	const Vector2 rightPos = pRightTrans->GetPosition() + pRight->GetOffset();

	const Vector2 leftSize = pLeft->GetSize();
	const Vector2 rightSize = pRight->GetSize();

	const EColliderType leftType = pLeft->GetColliderType();
	const EColliderType rightType = pRight->GetColliderType();

	if (leftType == EColliderType::BOX_2D && rightType == EColliderType::BOX_2D)
	{
		return fabs(leftPos.x - rightPos.x) < fabs(leftSize.x + rightSize.x) * 0.5f
			&& fabs(leftPos.y - rightPos.y) < fabs(leftSize.y + rightSize.y) * 0.5f;
	}

	if (leftType == EColliderType::CIRCLE_2D && rightType == EColliderType::CIRCLE_2D)
	{
		const Vector2 leftCirclePos = leftPos + leftSize * 0.5f;
		const Vector2 rightCirclePos = rightPos + rightSize * 0.5f;

		const float distanceSq = (leftCirclePos - rightCirclePos).Length();

		return distanceSq <= (leftSize.x + rightSize.y) * 0.5f;
	}

	return false;
}