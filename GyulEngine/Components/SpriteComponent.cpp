#include "SpriteComponent.h"

#include "../Core/Assert.h"

SpriteComponent::SpriteComponent()
	: Component(L"SpriteComponent")
	, mpTexture(nullptr)
	, mSize(0.f, 0.f)
{

}

void SpriteComponent::Initialize()
{
}

void SpriteComponent::Update()
{
}

void SpriteComponent::LateUpdate()
{
}

void SpriteComponent::Render() const
{
}
