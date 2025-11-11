#pragma once

#include <vector>
#include <string>

#include "UI/IEditorUIDrawable.h"

class Actor;
class MeshComponent;

class Scene final : public IEditorUIDrawable
{
public:
	Scene(const std::string& name);
	~Scene();

	void Update(const float deltaTime);
	void Render();

	void EnterPlayMode();
	void ExitPlayMode();

	void AddMeshComponent(MeshComponent* const pMeshComponent);
	void RemoveMeshComponent(MeshComponent* const pMeshComponent);

	virtual void DrawEditorUI() override;

private:
	std::string mSceneName;

	std::vector<Actor*> mpActorOriginals;
	std::vector<Actor*> mpPlayActors;
	std::vector<Actor*> mpPendingActors;

	std::vector<MeshComponent*> mpMeshComponents;
	std::vector<MeshComponent*> mpPendingMeshComponents;

	bool mbPlaying;

	int mNextActorId; // for generating unique actor names

private:
	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene& operator=(Scene&& other) = delete;
};