#pragma once

#include "MeshComponent.h"

class SpriteComponent final : public MeshComponent
{
public:
	SpriteComponent(Actor* const pOwner);
	virtual ~SpriteComponent() = default;
	SpriteComponent(const SpriteComponent& other) = delete;
	SpriteComponent& operator=(const SpriteComponent& other) = delete;
	SpriteComponent(SpriteComponent&& other) = delete;
	SpriteComponent& operator=(SpriteComponent&& other) = delete;

	virtual void Update(const float deltaTime) override;
	virtual void Draw(ID3D11DeviceContext& deviceContext) const;

private:
};