#include "Layer.h"

#include "../Core/Assert.h"
#include "Actor.h"

enum
{
	DEFAULT_BUFFER_SIZE = 8
};

Layer::Layer(const std::wstring& name)
	: Entity(name)
	, mpActors()
{
	mpActors.reserve(DEFAULT_BUFFER_SIZE);
}

Layer::~Layer()
{
	for (Actor* const pActor : mpActors)
	{
		delete pActor;
	}
}

void Layer::Initialize()
{
	for (Actor* const pActor : mpActors)
	{
		pActor->Initialize();
	}
}

void Layer::Update()
{
	for (Actor* const pActor : mpActors)
	{
		switch (pActor->GetState())
		{
		case Actor::EState::PAUSED:
		case Actor::EState::DEAD:
			continue;

		default:
			pActor->Update();
			break;
		}
	}
}

void Layer::LateUpdate()
{
	for (Actor* const pActor : mpActors)
	{
		switch (pActor->GetState())
		{
		case Actor::EState::PAUSED:
		case Actor::EState::DEAD:
			continue;

		default:
			pActor->LateUpdate();
			break;
		}
	}
}

void Layer::Render() const
{
	for (Actor* const pActor : mpActors)
	{
		pActor->Render();
	}
}

void Layer::AddActor(Actor* const pActor)
{
	ASSERT(pActor != nullptr);

	mpActors.push_back(pActor);
}

void Layer::RemoveActor(Actor* const pActor)
{
	for (auto iter = mpActors.begin(); iter != mpActors.end(); ++iter)
	{
		if (*iter == pActor)
		{
			mpActors.erase(iter);

			break;
		}
	}
}
