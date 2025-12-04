#pragma once

#include "Collider2DComponent.h"

class CircleCollider2DComponent final : public Collider2DComponent
{
public:
	CircleCollider2DComponent();
	virtual ~CircleCollider2DComponent() = default;

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() const override;

	EColliderType GetColliderType() const override
	{
		return EColliderType::CIRCLE_2D;
	}

private:
	float mRadius;

private:
	CircleCollider2DComponent(const CircleCollider2DComponent& other) = delete;
	CircleCollider2DComponent& operator=(const CircleCollider2DComponent& other) = delete;
	CircleCollider2DComponent(CircleCollider2DComponent&& other) noexcept = delete;
	CircleCollider2DComponent& operator=(CircleCollider2DComponent&& other) noexcept = delete;
};