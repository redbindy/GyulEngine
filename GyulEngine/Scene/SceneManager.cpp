#include "SceneManager.h"

#include "Scene.h"

enum
{
	DEFAULT_BUFFER_SIZE = 16
};

SceneManager* SceneManager::spInstance = nullptr;

SceneManager::SceneManager()
	: mpScenes()
	, mpActiveScene(nullptr)
	, mpDontDestroyOnLoad(nullptr)
{
	mpScenes.reserve(DEFAULT_BUFFER_SIZE);

	mpDontDestroyOnLoad = new Scene(L"Don't Destroy");
}

SceneManager::~SceneManager()
{
	for (std::pair<const std::wstring, Scene*> pair : mpScenes)
	{
		delete pair.second;
	}

	delete mpDontDestroyOnLoad;
}

void SceneManager::Update()
{
	if (mpActiveScene != nullptr)
	{
		mpActiveScene->Update();
	}

	mpDontDestroyOnLoad->Update();
}

void SceneManager::LateUpdate()
{
	if (mpActiveScene != nullptr)
	{
		mpActiveScene->LateUpdate();
	}

	mpDontDestroyOnLoad->LateUpdate();
}

void SceneManager::Render()
{
	if (mpActiveScene != nullptr)
	{
		mpActiveScene->Render();
	}

	mpDontDestroyOnLoad->Render();
}

Scene* SceneManager::LoadScene(const std::wstring& name)
{
	if (mpActiveScene != nullptr)
	{
		mpActiveScene->OnExit();
	}

	auto iter = mpScenes.find(name);

	if (iter == mpScenes.end())
	{
		return nullptr;
	}

	mpActiveScene = iter->second;
	mpActiveScene->OnEnter();

	return mpActiveScene;
}

void SceneManager::CreateInstance()
{
	ASSERT(spInstance == nullptr);

	spInstance = new SceneManager();
}
