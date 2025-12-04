#pragma once

#include "../Components/Component.h"
#include "Actor.h"
#include "Layer.h"
#include "Scene.h"
#include "SceneManager.h"
#include "../Core/MathHelper.h"

namespace object
{
	template<typename T>
	static T* Instantiate(const ELayerType type)
	{
		T* const pActor = new T();

		SceneManager& sceneManager = SceneManager::GetInstance();

		Scene* const pActiveScene = sceneManager.GetActiveScene();

		Layer* const pLayer = pActiveScene->GetLayer(type);
		pLayer->AddActor(pActor);

		return pActor;
	}

	template<typename T>
	static T* Instantiate(const ELayerType type, const Vector2 pos)
	{
		T* const pActor = new T();

		SceneManager& sceneManager = SceneManager::GetInstance();

		Scene* const pActiveScene = sceneManager.GetActiveScene();

		Layer* const pLayer = pActiveScene->GetLayer(type);
		pLayer->AddActor(pActor);

		// set transform
	}

	static void DontDestroyOnLoad(Actor* const pActor)
	{
		SceneManager& sceneManager = SceneManager::GetInstance();

		Scene* pActiveScene = sceneManager.GetActiveScene();

		pActiveScene->RemoveActor(pActor);

		Scene* pDontDestroyOnLoad = sceneManager.GetDontDestroyOnLoad();

		pDontDestroyOnLoad->AddActor(pActor->GetLayerType(), pActor);
	}
};
