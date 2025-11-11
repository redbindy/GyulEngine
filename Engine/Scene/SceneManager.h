#pragma once

#include <vector>
#include <string>

#include "Core/Assert.h"
#include "UI/IEditorUIDrawable.h"

class Scene;

class SceneManager final : IEditorUIDrawable
{
public:
	void CreateScene(const std::string& name);
	void RemoveScene(Scene* const pScene);

	virtual void DrawEditorUI() override;

	inline Scene* GetScene(const int index) const
	{
		ASSERT(index >= 0 && index < static_cast<int>(mpScenes.size()));

		return mpScenes[index];
	}

	// static
	static void Initialize();

	static SceneManager& GetInstance()
	{
		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static SceneManager* spInstance;

	std::vector<Scene*> mpScenes;
	int mSelectedSceneIndex; // currently selected scene index

private:
	SceneManager();
	~SceneManager();

private:
	SceneManager(const SceneManager& other) = delete;
	SceneManager& operator=(const SceneManager& other) = delete;
	SceneManager(SceneManager&& other) = delete;
	SceneManager& operator=(SceneManager&& other) = delete;
};