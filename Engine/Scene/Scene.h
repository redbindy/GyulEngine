#pragma once

#include <vector>
#include <string>

class Actor;
class MeshComponent;

class Scene final
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

private:
	std::string mSceneName;

	std::vector<Actor*> mpActorOriginals;
	std::vector<Actor*> mpPlayActors;

	std::vector<MeshComponent*> mpMeshComponents;

	bool mbPlaying;

private:
	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene(Scene&& other) = delete;
	Scene& operator=(Scene&& other) = delete;
};