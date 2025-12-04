#pragma once

#include <unordered_map>
#include <stack>
#include <queue>

#include "EUIType.h"

class UIBase;

class UIManager final
{
public:

	void Initialize();
	void Update();
	void LateUpdate();
	void Render() const;

	void OnLoad(const EUIType type);
	void OnComplete(UIBase* const pUI);
	void OnFail();

	void Push(const EUIType type);
	void Pop(const EUIType type);

	// static
	static void CreateInstance();

	static UIManager& GetInstance()
	{
		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static UIManager* spInstance;

	std::unordered_map<EUIType, UIBase*> mUIMap;

	std::vector<UIBase*> mUIBases;
	std::queue<EUIType> mRequestUIQueue;

	UIBase* mpActiveUI;

private:
	UIManager();
	virtual ~UIManager() = default;
};