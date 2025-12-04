#pragma once

#include "../Scene/SceneManager.h"
#include "PlayScene.h"
#include "TitleScene.h"

inline void LoadScenes()
{
	SceneManager& sceneManager = SceneManager::GetInstance();

	sceneManager.CreateScene<TitleScene>(L"TitleScene");
	sceneManager.CreateScene<PlayScene>(L"PlayScene");

	sceneManager.LoadScene(L"PlayScene");
}