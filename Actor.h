#pragma once

#include <string>

#include "SimpleMath.h"

#include "CommonDefs.h"
#include "IUIDrawable.h"
#include "Component/ComponentGenerator.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Component;

class Actor final : public IUIDrawable
{
public:
	Actor(const char* const label);
	~Actor();
	Actor(const Actor&) = delete;
	Actor& operator=(const Actor&) = delete;
	Actor(Actor&&) = delete;
	Actor& operator=(Actor&&) = delete;

	void Update(const float deltaTime);
	Matrix GetTransform() const;

	void AddComponent(Component* const pComponent);
	void RemoveComponent(Component* const pComponent);

	virtual void DrawUI() override;

	inline const char* GetLabel() const
	{
		return mLabel;
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

	inline Vector3 GetRotation() const
	{
		return mRotation;
	}

	inline void SetPosition(const Vector3& pos)
	{
		mPosition = pos;
	}

	inline void SetRotation(const Vector3& rotation)
	{
		mRotation = rotation;
	}

private:
	char mLabel[MAX_LABEL_LENGTH];
	char mTempBuffer[MAX_LABEL_LENGTH];
	bool mbRenaming;
	bool mbAlive;

	// transform
	Vector3 mPosition;
	Vector3 mScale;
	Vector3 mRotation;

	// component
	std::vector<Component*> mComponents;
	std::vector<Component*> mPendingComponents;

	bool mHasComponent[ComponentGenerator::GetComponentCount()];

	BoundingSphere mBoudingSphere;
};