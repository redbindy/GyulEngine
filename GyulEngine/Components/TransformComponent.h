#pragma once

#include "Component.h"
#include "../Core/MathHelper.h"

class TransformComponent final : public Component
{
public:
	TransformComponent();
	virtual ~TransformComponent() = default;

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() const override;

	Vector2 GetPosition() const
	{
		return mPosition;
	}

	void SetPosition(const Vector2 position)
	{
		mPosition = position;
	}

	Vector2 GetScale() const
	{
		return mScale;
	}

	void SetScale(const Vector2 scale)
	{
		mScale = scale;
	}

	float GetRotation() const
	{
		return mRotation;
	}

	void SetRotation(const float rotation)
	{
		mRotation = rotation;
	}

private:
	Vector2 mPosition;
	Vector2 mScale;
	float mRotation;

private:
	TransformComponent(const TransformComponent& other) = delete;
	TransformComponent& operator=(const TransformComponent& other) = delete;
	TransformComponent(TransformComponent&& other) noexcept = delete;
	TransformComponent& operator=(TransformComponent&& other) noexcept = delete;
};