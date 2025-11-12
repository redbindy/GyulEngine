#include "SceneManager.h"

#include "Scene.h"
#include "Core/Assert.h"
#include "Components/ComponentFactory.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"
#include "Core/GameCore.h" // for setting current scene

enum
{
	DEFAULT_SCENE_CAPACITY = 8
};

SceneManager* SceneManager::spInstance = nullptr;

SceneManager::SceneManager()
	: mpScenes()
	, mSelectedSceneIndex(0)
{
	mpScenes.reserve(DEFAULT_SCENE_CAPACITY);

	CreateScene("Default");
}

SceneManager::~SceneManager()
{
	ComponentFactory::Destroy();

	for (Scene* pScene : mpScenes)
	{
		delete pScene;
	}
}

void SceneManager::CreateScene(const std::string& name)
{
	Scene* const pNewScene = new Scene(name);

	mpScenes.push_back(pNewScene);
}

void SceneManager::RemoveScene(Scene* const pScene)
{
	ASSERT(pScene != nullptr);

#define VECTOR_ITER std::vector<Scene*>::iterator

	VECTOR_ITER iter = std::find(mpScenes.begin(), mpScenes.end(), pScene);
	if (iter != mpScenes.end())
	{
		delete* iter;

		mpScenes.erase(iter);
	}

#undef VECTOR_ITER
}

void SceneManager::DrawEditorUI()
{
	ImGui::PushID("SceneManager");

	// 단순한 씬 목록 표시 및 선택
	// 현재 선택된 씬은 라디오 버튼처럼 표시
	for (int i = 0; i < static_cast<int>(mpScenes.size()); ++i)
	{
		ImGui::PushID(i);

		bool selected = (i == mSelectedSceneIndex);
		if (ImGui::Selectable(UTF8_TEXT("씬"), selected))
		{
			mSelectedSceneIndex = i;
			GameCore::GetInstance().SetCurrentScene(mpScenes[mSelectedSceneIndex]);
		}

		ImGui::SameLine();
		ImGui::Text("%d", i); // index 표시 (숫자는 그대로)

		ImGui::PopID();
	}

	ImGui::PopID();
}

Scene* SceneManager::GetScene(const std::string& name) const
{
	Scene* pRet = nullptr;

	for (Scene* const pScene : mpScenes)
	{
		if (pScene->GetName() == name)
		{
			pRet = pScene;

			break;
		}
	}

	ASSERT(pRet != nullptr);

	return pRet;
}

void SceneManager::Initialize()
{
	ASSERT(spInstance == nullptr);

	ComponentFactory::Initialize();

	spInstance = new SceneManager();
}