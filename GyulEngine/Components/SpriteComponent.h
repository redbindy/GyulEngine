#pragma once

#include "Component.h"
#include "../Core/MathHelper.h"

class Texture;

class SpriteComponent final : public Component
{
public:
	SpriteComponent();
	virtual ~SpriteComponent() = default;

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void Render() const override;

	void SetTexture(Texture* const pTexture)
	{
		mpTexture = pTexture;
	}

	void SetSize(const Vector2 size)
	{
		mSize = size;
	}

private:
	Texture* mpTexture;
	Vector2 mSize;

private:
	SpriteComponent(const SpriteComponent& other) = delete;
	SpriteComponent& operator=(const SpriteComponent& other) = delete;
	SpriteComponent(SpriteComponent&& other) noexcept = delete;
	SpriteComponent& operator=(SpriteComponent&& other) noexcept = delete;
};