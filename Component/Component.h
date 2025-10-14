#pragma once

#include "IUIDrawable.h"

class Actor;

class Component : public IUIDrawable
{
public:
	Component(Actor* const pOwner);
	virtual ~Component() = default;
	Component(const Component& other) = delete;
	Component& operator=(const Component& other) = delete;
	Component(Component&& other) = delete;
	Component& operator=(Component&& other) = delete;

	virtual void Update(const float deltaTime) = 0;
	virtual void DrawUI() = 0;

	inline const char* GetLabel() const
	{
		return mLabel;
	}

	inline Actor* GetOwner() const
	{
		return mpOwner;
	}

protected:
	Component(Actor* const pOwner, const char* const label);

private:
	const char* mLabel;

	Actor* const mpOwner;
};