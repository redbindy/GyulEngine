#include "Scene.h"

#include "Actor.h"
#include "Core/Assert.h"
#include "Components/MeshComponent.h"
#include "Components/ComponentFactory.h"

enum
{
	DEFAULT_ACTOR_ORIGINALS_SIZE = 32
};

Scene::Scene(const std::string& name)
	: mSceneName(name)
	, mpActorOriginals()
	, mpPlayActors()
	, mbPlaying(false)
{
	mpActorOriginals.reserve(DEFAULT_ACTOR_ORIGINALS_SIZE);
	mpPlayActors.reserve(DEFAULT_ACTOR_ORIGINALS_SIZE);

	Actor* const pActor = new Actor(this, "DefaultActor");

	ComponentFactory& componentFactory = ComponentFactory::GetInstance();
	componentFactory.CreateComponentAlloc("MeshComponent", pActor);

	mpActorOriginals.push_back(pActor);
}

Scene::~Scene()
{
	for (Actor* const pActor : mpActorOriginals)
	{
		delete pActor;
	}

	for (Actor* const pActor : mpPlayActors)
	{
		delete pActor;
	}
}

void Scene::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	if (mbPlaying)
	{
		for (Actor* const pActor : mpPlayActors)
		{
			pActor->Update(deltaTime);
		}
	}
}

void Scene::Render()
{
	for (MeshComponent* const pMeshComponent : mpMeshComponents)
	{
		pMeshComponent->RequestRender();
	}
}

void Scene::EnterPlayMode()
{
	bool mbPlaying = true;

	for (Actor* const pActorOriginal : mpActorOriginals)
	{
		Actor* const pPlayActor = new Actor(*pActorOriginal);

		mpPlayActors.push_back(pPlayActor);
	}
}

void Scene::ExitPlayMode()
{
	mbPlaying = false;

	for (Actor* const pPlayActor : mpPlayActors)
	{
		delete pPlayActor;
	}
	mpPlayActors.clear();
}

void Scene::AddMeshComponent(MeshComponent* const pMeshComponent)
{
	ASSERT(pMeshComponent != nullptr);

	mpMeshComponents.push_back(pMeshComponent);
}

void Scene::RemoveMeshComponent(MeshComponent* const pMeshComponent)
{
	ASSERT(pMeshComponent != nullptr);

#define VECTOR_ITER std::vector<MeshComponent*>::iterator

	VECTOR_ITER iter = std::find(mpMeshComponents.begin(), mpMeshComponents.end(), pMeshComponent);

	if (iter != mpMeshComponents.end())
	{
		mpMeshComponents.erase(iter);
	}

#undef VECTOR_ITER
}

