#pragma once

#include <vector>

#include "../Core/Assert.h"
#include "../Core/Entity.h"
#include "ELayerType.h"

class Actor;
class Layer;

class Scene : public Entity
{
public:
	Scene(const std::wstring& name);
	virtual ~Scene();

	virtual void Initialize();
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render() const;

	virtual void OnEnter();
	virtual void OnExit();

	void AddActor(const ELayerType type, Actor* const pActor);
	void RemoveActor(Actor* const pActor);

	Layer* GetLayer(const ELayerType type) const
	{
		return mpLayers[static_cast<size_t>(type)];
	}

private:
	std::vector<Layer*> mpLayers;

private:
	Scene(const Scene& other) = delete;
	Scene& operator=(const Scene& other) = delete;
	Scene(Scene&& other) noexcept = delete;
	Scene& operator=(Scene&& other) noexcept = delete;
};