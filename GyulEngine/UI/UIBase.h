#pragma once

#include "../Core/Entity.h"
#include "EUIType.h"

class UIBase : public Entity
{
public:
	UIBase() = default;
	virtual ~UIBase() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual void Render() const = 0;

	virtual void Activate() = 0;
	virtual void Deactivate() = 0;

	virtual void UIClear() = 0;

	virtual void OnInitialize() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnLateUpdate() = 0;
	virtual void OnRender() const = 0;
	virtual void OnClear() = 0;
	virtual void OnActive() = 0;
	virtual void OnDeactive() = 0;

	virtual EUIType GetType() const = 0;

	void SetFullScreen(const bool bFullScreen)
	{
		mbFullScreen = bFullScreen;
	}

	bool IsFullScreen() const
	{
		return mbFullScreen;
	}

private:
	bool mbFullScreen;
	bool mbEnabled;
};