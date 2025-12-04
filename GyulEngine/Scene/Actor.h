#pragma once

#include <vector>

#include "../Core/Entity.h"
#include "../Components/Component.h"
#include "ELayerType.h"

class Actor : public Entity
{
public:
	enum class EState
	{
		ACTIVE,
		PAUSED,
		DEAD,
		COUNT
	};

public:
	Actor(const std::wstring& name);
	virtual ~Actor();

	virtual void Initialize();
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render() const;

	// template
	template<typename T>
	T* AddComponent()
	{
		T* pComponent = new T(this);

		mpComponents.push_back(pComponent);

		return pComponent;
	}

	template<typename T>
	T* GetComponent() const
	{
		T* pComponent = nullptr;
		for (Component* const pComp : mpComponents)
		{
			pComponent = dynamic_cast<T*>(pComp);

			if (pComponent != nullptr)
			{
				break;
			}
		}

		return pComponent;
	}

	EState GetState() const
	{
		return mState;
	}

	void SetActive()
	{
		mState = EState::ACTIVE;
	}

	void SetPaused()
	{
		mState = EState::PAUSED;
	}

	void SetDead()
	{
		mState = EState::DEAD;
	}

	ELayerType GetLayerType() const
	{
		return mLayerType;
	}

	void SetLayerType(const ELayerType type)
	{
		mLayerType = type;
	}

private:
	EState mState;

	std::vector<Component*> mpComponents;

	ELayerType mLayerType;

private:
	Actor(const Actor& other) = delete;
	Actor& operator=(const Actor& other) = delete;
	Actor(Actor&& other) noexcept = delete;
	Actor& operator=(Actor&& other) noexcept = delete;
};