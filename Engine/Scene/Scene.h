#pragma once

#include <vector>
#include <string>

#include "UI/IEditorUIDrawable.h"

class Actor;
class CameraComponent;

class Scene final : public IEditorUIDrawable
{
public:
	Scene(const std::string& name);
	~Scene();

	void Update(const float deltaTime);

	void EnterPlayMode();
	void ExitPlayMode();

	virtual void DrawEditorUI() override;

	const std::string& GetName() const
	{
		return mSceneName;
	}

private:
	std::string mSceneName;

	std::vector<Actor*> mpActorOriginals;
	std::vector<Actor*> mpPlayActors;
	std::vector<Actor*> mpPendingActors;

	int mNextActorId; // for generating unique actor names

private:
	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene& operator=(Scene&& other) = delete;
};