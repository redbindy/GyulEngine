#pragma once

#include "../Core/Entity.h"
#include "EComponentType.h"

class Actor;

class Component : public Entity
{
public:
	Component(const std::wstring& name);
	virtual ~Component() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual void Render() const = 0;

	virtual EComponentType GetComponentType() const = 0;

	Actor* GetOwner() const
	{
		return mpOwner;
	}

	void SetOwner(Actor* const pOwner)
	{
		mpOwner = pOwner;
	}

private:
	Actor* mpOwner;

private:
	Component(const Component& other) = delete;
	Component& operator=(const Component& other) = delete;
	Component(Component&& other) noexcept = delete;
	Component& operator=(Component&& other) noexcept = delete;
};