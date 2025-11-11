#include "Scene.h"

#include "Actor.h"
#include "Core/Assert.h"
#include "Components/MeshComponent.h"
#include "Components/ComponentFactory.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"

enum
{
	DEFAULT_ACTOR_BUFFER_SIZE = 32
};

Scene::Scene(const std::string& name)
	: mSceneName(name)
	, mpActorOriginals()
	, mpPlayActors()
	, mpPendingActors()
	, mpMeshComponents()
	, mbPlaying(false)
	, mNextActorId(0)
{
	mpActorOriginals.reserve(DEFAULT_ACTOR_BUFFER_SIZE);
	mpPlayActors.reserve(DEFAULT_ACTOR_BUFFER_SIZE);
	mpPendingActors.reserve(DEFAULT_ACTOR_BUFFER_SIZE);
	mpMeshComponents.reserve(DEFAULT_ACTOR_BUFFER_SIZE);

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
	mbPlaying = true;

	mpMeshComponents.swap(mpPendingMeshComponents);

	for (Actor* const pActorOriginal : mpActorOriginals)
	{
		Actor* const pPlayActor = new Actor(this, pActorOriginal->GetLabel());

		*pPlayActor = *pActorOriginal;

		mpPlayActors.push_back(pPlayActor);
	}
}

void Scene::ExitPlayMode()
{
	mbPlaying = false;

	mpMeshComponents.swap(mpPendingMeshComponents);
	mpPendingMeshComponents.clear();

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

void Scene::DrawEditorUI()
{
	ImGui::PushID(mSceneName.c_str());

	if (ImGui::Begin(mSceneName.c_str()))
	{
		// Add Actor button
		if (ImGui::Button(UTF8_TEXT("액터 추가")))
		{
			// generate unique name
			char nameBuf[MAX_LABEL_LENGTH];
			sprintf(nameBuf, "Actor%d", mNextActorId++);

			Actor* const pNewActor = new Actor(this, nameBuf);
			mpActorOriginals.push_back(pNewActor);
		}

		ImGui::Separator();

		for (int i = 0; i < mpActorOriginals.size(); ++i)
		{
			ImGui::PushID(i);

			Actor* const pActor = mpActorOriginals[i];

			if (ImGui::Button(UTF8_TEXT("액터 삭제")))
			{
				delete pActor;
			}
			else
			{
				mpPendingActors.push_back(pActor);

				pActor->DrawEditorUI();
			}

			ImGui::PopID();
		}

		mpActorOriginals.swap(mpPendingActors);
		mpPendingActors.clear();
	}
	ImGui::End();

	ImGui::PopID();
}

