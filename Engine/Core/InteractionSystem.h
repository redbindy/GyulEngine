#pragma once

#include <unordered_map>
#include <vector>

#include "Assert.h"
#include "MathHelper.h"

class Actor;

struct InteractionCollider
{
	Actor* pActorOrNull;
	BoundingSphere boundingSphereLocal;
};

class InteractionSystem final
{
public:
	void Pick(const std::string& scene);
	void ReleasePick();
	void Update();

	void MakeSceneBuffer(const std::string& sceneName);
	void RemoveSceneBuffer(const std::string& sceneName);

	void RegisterCollider(
		const std::string& scene,
		Actor* const pActor,
		const BoundingSphere& localSphere
	);

	void UnregisterCollider(
		const std::string& scene,
		Actor* const pActor
	);

	static void Initialize();

	static InteractionSystem& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static InteractionSystem* spInstance;

	std::unordered_map<std::string, std::vector<InteractionCollider>> mCollidersMap;

	Vector3 mMouseStartWorld;
	Vector3 mMouseEndWorld;
	Ray mMouseRay;
	float mCollisionDist;

	bool mbPicked;
	float mPrevRatio;
	Vector3 mPrevVector;

	InteractionCollider mPickedCollider;

private:
	InteractionSystem();
	~InteractionSystem() = default;

	void updateInteractionInfo();

private:
	InteractionSystem(const InteractionSystem& other) = delete;
	InteractionSystem& operator=(const InteractionSystem& other) = delete;
	InteractionSystem(InteractionSystem&& other) = delete;
	InteractionSystem& operator=(InteractionSystem&& other) = delete;
};