#pragma once

#include <bitset>
#include <unordered_map>

#include "../Core/Assert.h"
#include "ELayerType.h"

class Scene;
class Collider2DComponent;

class CollisionManager final
{
public:
	union CollisionID
	{
		struct
		{
			uint32_t left;
			uint32_t right;
		};

		uint64_t id;
	};

public:
	void Initialize();
	void Update();
	void LateUpdate();
	void Render() const;

	void MaskCollsionLayer(
		const ELayerType left,
		const ELayerType right,
		const bool bEnable
	);

	void CheckLayerCollision(
		Scene* const pScene,
		const ELayerType left,
		const ELayerType right
	);

	// static
	static void CreateInstance();

	static CollisionManager& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static CollisionManager* spInstance;

	std::bitset<GetLayerTypeCount()> mCollisionMatrix[GetLayerTypeCount()];
	std::unordered_map<uint64_t, bool> mCollisionMap;

private:
	CollisionManager();
	~CollisionManager() = default;

	bool intersect(Collider2DComponent* pLeft, Collider2DComponent* pRight);

private:
	CollisionManager(const CollisionManager& other) = delete;
	CollisionManager& operator=(const CollisionManager& other) = delete;
	CollisionManager(CollisionManager&& other) noexcept = delete;
	CollisionManager& operator=(CollisionManager&& other) noexcept = delete;
};