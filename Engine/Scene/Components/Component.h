#pragma once

#include <cstdint>

#include "Core/Assert.h"
#include "UI/IEditorUIDrawable.h"

class Actor;

class Component : public IEditorUIDrawable
{
public:
	Component(Actor* const pOwner);
	Component(Actor* const pOwner, const char* const label, const uint32_t updateOrder = 10u);
	virtual ~Component() = default;

	virtual void CloneFrom(const Component& other);

	virtual void Update(const float deltaTime) = 0;

	virtual void DrawEditorUI() override = 0;

	inline const char* GetLabel() const
	{
		return mLabel;
	}

	inline bool IsAlive() const
	{
		return mbAlive;
	}

	inline Actor& GetOwner() const
	{
		ASSERT(mpOwner != nullptr);

		return *mpOwner;
	}

	inline uint32_t GetUpdateOrder() const
	{
		return mUpdateOrder;
	}

private:
	const char* mLabel;

	bool mbAlive;

	uint32_t mUpdateOrder;

	Actor* mpOwner;

private:
	Component(const Component& other) = delete;
	Component& operator=(const Component& other) = delete;
	Component(Component&& other) = delete;
	Component& operator=(Component&& other) = delete;
};