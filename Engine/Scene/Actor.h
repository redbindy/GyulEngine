#pragma once

#include <vector>
#include <memory>

#include "Core/MathHelper.h"
#include "Core/CommonDefs.h"

class Component;
class Scene;

class Actor final
{
public:
	Actor(Scene* const pScene, const char* const label);
	~Actor();

	Actor(const Actor& other);
	Actor& operator=(const Actor& other);

	void Update(const float deltaTime);
	Matrix GetTransform() const;

	void AddComponent(Component* const pComponent);
	void RemoveComponent(Component* const pComponent);

	inline const char* GetLabel() const
	{
		return mLabel;
	}

	inline Scene& GetScene() const
	{
		return *mpScene;
	}

	inline bool IsAlive() const
	{
		return mbAlive;
	}

	inline Vector3 GetPosition() const
	{
		return mPosition;
	}

	inline Vector3 GetScale() const
	{
		return mScale;
	}

	inline Quaternion GetRotation() const
	{
		return mRotation;
	}

	inline void SetPosition(const Vector3& pos)
	{
		mPosition = pos;
	}

	inline void SetRotation(const Quaternion& rotation)
	{
		mRotation = rotation;
	}

private:
	char mLabel[MAX_LABEL_LENGTH];
	char mTempBuffer[MAX_LABEL_LENGTH];
	bool mbRenaming;
	bool mbAlive;

	Scene* mpScene;

	// transform
	Vector3 mPosition;
	Vector3 mScale;
	Quaternion mRotation;

	// component
	std::vector<Component*> mComponents;
	std::vector<Component*> mPendingComponents;

private:
	Actor(Actor&& other) = delete;
	Actor& operator=(Actor&& other) = delete;
};