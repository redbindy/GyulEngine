#include "UIManager.h"

#include "UIBase.h"
#include "../Core/Assert.h"

enum
{
	DEFAULT_BUFFER_SIZE = 16
};

UIManager* UIManager::spInstance = nullptr;

UIManager::UIManager()
	: mUIMap()
	, mUIBases()
	, mpActiveUI(nullptr)
{
	mUIMap.reserve(DEFAULT_BUFFER_SIZE);
	mUIBases.reserve(DEFAULT_BUFFER_SIZE);
}

void UIManager::Initialize()
{
}

void UIManager::Update()
{
	std::vector<UIBase*> tempUIs = mUIBases;
	while (tempUIs.size() > 0)
	{
		UIBase* const pUI = tempUIs.back();
		tempUIs.pop_back();

		pUI->Update();
	}

	if (mRequestUIQueue.size() > 0)
	{
		const EUIType requestType = mRequestUIQueue.front();

		mRequestUIQueue.pop();

		OnLoad(requestType);
	}
}

void UIManager::LateUpdate()
{
	std::vector<UIBase*> tempUIs = mUIBases;
	while (tempUIs.size() > 0)
	{
		UIBase* const pUI = tempUIs.back();
		tempUIs.pop_back();

		pUI->LateUpdate();
	}
}

void UIManager::Render() const
{
	std::vector<UIBase*> tempUIs = mUIBases;
	while (tempUIs.size() > 0)
	{
		UIBase* const pUI = tempUIs.back();
		tempUIs.pop_back();

		pUI->Render();
	}
}

void UIManager::OnLoad(const EUIType type)
{
	auto iter = mUIMap.find(type);

	if (iter == mUIMap.end())
	{
		OnFail();

		return;
	}

	OnComplete(iter->second);
}

void UIManager::OnComplete(UIBase* const pUI)
{
	ASSERT(pUI != nullptr);

	pUI->Initialize();
	pUI->Activate();
	pUI->Update();

	if (pUI->IsFullScreen())
	{
		std::vector<UIBase*> tempUIs = mUIBases;

		while (!tempUIs.empty())
		{
			UIBase* const pTempUI = tempUIs.back();
			tempUIs.pop_back();

			if (pTempUI != nullptr)
			{
				pTempUI->Deactivate();
			}
		}
	}

	mUIBases.push_back(pUI);
	mpActiveUI = nullptr;
}

void UIManager::OnFail()
{
	mpActiveUI = nullptr;
}

void UIManager::Push(const EUIType type)
{
	mRequestUIQueue.push(type);
}

void UIManager::Pop(const EUIType type)
{
	if (mUIBases.size() == 0)
	{
		return;
	}

	while (mUIBases.size() > 0)
	{
		mUIBases.pop_back();
	}
}

void UIManager::CreateInstance()
{
	ASSERT(spInstance == nullptr);

	spInstance = new UIManager();
}