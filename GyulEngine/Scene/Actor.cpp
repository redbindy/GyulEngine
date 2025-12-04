#include "Actor.h"

Actor::Actor(const std::wstring& name)
	: Entity(name)
	, mState(EState::ACTIVE)
	, mpComponents()
	, mLayerType(ELayerType::NONE)
{
	mpComponents.reserve(GetComponentTypeCount());
}

Actor::~Actor()
{
	for (Component* const pComponent : mpComponents)
	{
		delete pComponent;
	}
}

void Actor::Initialize()
{
	for (Component* const pComponent : mpComponents)
	{
		pComponent->Initialize();
	}
}

void Actor::Update()
{
	for (Component* const pComponent : mpComponents)
	{
		pComponent->Update();
	}
}

void Actor::LateUpdate()
{
	for (Component* const pComponent : mpComponents)
	{
		pComponent->LateUpdate();
	}
}

void Actor::Render() const
{
	for (Component* const pComponent : mpComponents)
	{
		pComponent->Render();
	}
}
