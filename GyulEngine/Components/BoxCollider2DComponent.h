#pragma once

#include "../Core/MathHelper.h"

#include "Collider2DComponent.h"

class BoxCollider2DComponent final : public Collider2DComponent
{
public:
	BoxCollider2DComponent();
	virtual ~BoxCollider2DComponent() = default;

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() const override;

	EColliderType GetColliderType() const override
	{
		return EColliderType::BOX_2D;
	}

private:
	Vector2 mSize;
};