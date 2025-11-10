#include "SceneManager.h"

#include "Scene.h"
#include "Core/Assert.h"
#include "Components/ComponentFactory.h"

enum
{
	DEFAULT_SCENE_CAPACITY = 8
};

SceneManager* SceneManager::spInstance = nullptr;

SceneManager::SceneManager()
	: mpScenes()
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

void SceneManager::Initialize()
{
	ASSERT(spInstance == nullptr);

	ComponentFactory::Initialize();

	spInstance = new SceneManager();
}