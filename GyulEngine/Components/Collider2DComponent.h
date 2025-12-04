#pragma once

#include "../Core/MathHelper.h"
#include "Component.h"
#include "EColliderType.h"

class Collider2DComponent : public Component
{
public:
	virtual ~Collider2DComponent() = default;

	void Initialize() override = 0;
	void Update() override = 0;
	void LateUpdate() override = 0;
	void Render() const override = 0;

	virtual void OnCollisionEnter(Collider2DComponent* const pOther);
	virtual void OnCollisionStay(Collider2DComponent* const pOther);
	virtual void OnCollisionExit(Collider2DComponent* const pOther);

	virtual EColliderType GetColliderType() const = 0;

	UINT GetID() const
	{
		return mID;
	}

	Vector2 GetOffset() const
	{
		return mOffset;
	}

	void SetOffset(const Vector2 offset)
	{
		mOffset = offset;
	}

	Vector2 GetSize() const
	{
		return mSize;
	}

	void SetSize(const Vector2 size)
	{
		mSize = size;
	}

protected:
	Collider2DComponent(const std::wstring& name);

private:
	static UINT sCollisionID;

	UINT mID;
	Vector2 mOffset;
	Vector2 mSize;
};