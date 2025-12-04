#include "TransformComponent.h"

#include "../Core/Assert.h"

TransformComponent::TransformComponent()
	: Component(L"Transform")
	, mPosition(0.f, 0.f)
	, mScale(1.f, 1.f)
	, mRotation(0.f)
{

}

void TransformComponent::Initialize()
{
}

void TransformComponent::Update()
{
}

void TransformComponent::LateUpdate()
{
}

void TransformComponent::Render() const
{
}
