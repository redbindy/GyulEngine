#include "Scene.h"

#include "Actor.h"
#include "Core/Assert.h"
#include "Components/ComponentFactory.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"
#include "Renderer/Renderer.h"
#include "Core/InteractionSystem.h"

enum
{
	DEFAULT_ACTOR_BUFFER_SIZE = 32
};

Scene::Scene(const std::string& name)
	: mSceneName(name)
	, mpActorOriginals()
	, mpPlayActors()
	, mpPendingActors()
	, mNextActorId(0)
{
	mpActorOriginals.reserve(DEFAULT_ACTOR_BUFFER_SIZE);
	mpPlayActors.reserve(DEFAULT_ACTOR_BUFFER_SIZE);
	mpPendingActors.reserve(DEFAULT_ACTOR_BUFFER_SIZE);

	Renderer& renderer = Renderer::GetInstance();

	renderer.AddMeshComponentList(mSceneName);

	InteractionSystem& interactionSystem = InteractionSystem::GetInstance();

	interactionSystem.MakeSceneBuffer(mSceneName);

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

	InteractionSystem& interactionSystem = InteractionSystem::GetInstance();

	interactionSystem.RemoveSceneBuffer(mSceneName);

	Renderer& renderer = Renderer::GetInstance();

	renderer.RemoveMeshComponentList(mSceneName);
}

void Scene::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	for (Actor* const pActor : mpPlayActors)
	{
		pActor->Update(deltaTime);
	}
}

void Scene::EnterPlayMode()
{
	for (Actor* const pActorOriginal : mpActorOriginals)
	{
		Actor* const pPlayActor = new Actor(this, pActorOriginal->GetLabel());

		*pPlayActor = *pActorOriginal;

		mpPlayActors.push_back(pPlayActor);
	}
}

void Scene::ExitPlayMode()
{
	for (Actor* const pPlayActor : mpPlayActors)
	{
		delete pPlayActor;
	}
	mpPlayActors.clear();
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
