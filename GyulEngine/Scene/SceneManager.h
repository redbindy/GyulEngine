#pragma once

#include <unordered_map>

#include "../Core/Assert.h"

class Scene;

class SceneManager final
{
public:
	void Update();
	void LateUpdate();
	void Render();

	template<typename T>
	Scene* CreateScene(const std::wstring& name)
	{
		T* pScene = new T(name);

		mpScenes.insert({ name, pScene });

		return pScene;
	}

	Scene* LoadScene(const std::wstring& name);

	Scene* GetActiveScene() const
	{
		return mpActiveScene;
	}

	Scene* GetDontDestroyOnLoad() const
	{
		return mpDontDestroyOnLoad;
	}

	// static
	static void CreateInstance();

	static SceneManager& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static SceneManager* spInstance;

	std::unordered_map<std::wstring, Scene*> mpScenes;
	Scene* mpActiveScene;
	Scene* mpDontDestroyOnLoad;

private:
	SceneManager();
	virtual ~SceneManager();

private:
	SceneManager(const SceneManager& other) = delete;
	SceneManager& operator=(const SceneManager& other) = delete;
	SceneManager(SceneManager&& other) noexcept = delete;
	SceneManager& operator=(SceneManager&& other) noexcept = delete;
};