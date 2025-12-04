#pragma once

#include <vector>

#include "../Core/Entity.h"

class Actor;

class Layer final : public Entity
{
public:
	Layer(const std::wstring& name);
	virtual ~Layer();

	virtual void Initialize();
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render() const;

	void AddActor(Actor* const pActor);
	void RemoveActor(Actor* const pActor);

	const std::vector<Actor*>& GetActors() const
	{
		return mpActors;
	}

private:
	std::vector<Actor*> mpActors;

private:
	Layer(const Layer& other) = delete;
	Layer& operator=(const Layer& other) = delete;
	Layer(Layer&& other) noexcept = delete;
	Layer& operator=(Layer&& other) noexcept = delete;
};