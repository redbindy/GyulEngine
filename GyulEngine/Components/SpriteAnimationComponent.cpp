#include "SpriteAnimationComponent.h"

#include <filesystem>

#include "../Resources/SpriteAnimation.h"
#include "../Core/Event.h"
#include "../Resources/ResourceManager.h"
#include "../Resources/Texture.h"

SpriteAnimationComponent::SpriteAnimationComponent()
	: Component(L"SpriteAnimationComponent")
	, mpAnimations()
	, mpActiveAnimation(nullptr)
	, mbLoop(false)
{
}

SpriteAnimationComponent::~SpriteAnimationComponent()
{
	for (auto& pair : mEvents)
	{
		delete pair.second;
	}

	for (auto& pair : mpAnimations)
	{
		delete pair.second;
	}
}

void SpriteAnimationComponent::Initialize()
{

}

void SpriteAnimationComponent::Update()
{
	if (mpActiveAnimation != nullptr)
	{
		mpActiveAnimation->Update();

		Events* pEvents = FindEvents(mpActiveAnimation->GetName());

		if (mpActiveAnimation->IsComplete())
		{
			if (pEvents != nullptr)
			{
				pEvents->completeEvent();
			}

			if (mbLoop)
			{
				mpActiveAnimation->Reset();
			}
		}
	}
}

void SpriteAnimationComponent::LateUpdate()
{
}

void SpriteAnimationComponent::Render() const
{
	if (mpActiveAnimation != nullptr)
	{
		mpActiveAnimation->Render();
	}
}

void SpriteAnimationComponent::CreateAnimation(
	const std::wstring& name,
	Texture* const pSpriteSheet,
	const Vector2 leftTop,
	const Vector2 size,
	const Vector2 offset,
	const int spriteLength,
	const float duration
)
{
	SpriteAnimation* pAnimation = FindAnimationOrNull(name);

	if (pAnimation != nullptr)
	{
		return;
	}

	pAnimation = new SpriteAnimation();
	pAnimation->CreateAnimation(
		name,
		pSpriteSheet,
		leftTop,
		size,
		offset,
		spriteLength,
		duration
	);

	pAnimation->SetSpriteAnimationComponent(this);

	Events* pEvents = new Events();
	mEvents.insert({ name, pEvents });

	mpAnimations.insert({ name,  pAnimation });
}

void SpriteAnimationComponent::CreateAnimationByFolder(
	const std::wstring& name,
	const std::wstring& path,
	const Vector2 offset,
	const float duration
)
{
	SpriteAnimation* pAnimation = FindAnimationOrNull(name);
	if (pAnimation != nullptr)
	{
		return;
	}

	int fileCount = 0;
	std::filesystem::path fs(path);

	std::vector<Texture*> pTextures;
	pTextures.reserve(4);

	for (auto& p : std::filesystem::recursive_directory_iterator(fs))
	{
		std::wstring fileName = p.path().filename();
		std::wstring fullName = p.path();

		ResourceManager& resourceManager = ResourceManager::GetInstance();

		Texture* pTexture = resourceManager.Load<Texture>(fileName, fullName);
		pTextures.push_back(pTexture);

		++fileCount;
	}

	const uint32_t imageWidth = pTextures[0]->GetWidth();
	const uint32_t imageHeight = pTextures[0]->GetHeight();

	const uint32_t sheetWidth = imageWidth * fileCount;
	const uint32_t sheetHeight = imageHeight;
	Texture* const pSpriteSheet = new Texture(name, sheetWidth, sheetHeight);

	for (int i = 0; i < pTextures.size(); ++i)
	{
		// copty each texture to sprite sheet
	}

	CreateAnimation(
		name,
		pSpriteSheet,
		Vector2(0.0f, 0.0f),
		Vector2(static_cast<float>(imageWidth), static_cast<float>(imageHeight)),
		offset,
		fileCount,
		duration
	);
}

SpriteAnimation* SpriteAnimationComponent::FindAnimationOrNull(const std::wstring& name) const
{
	auto iter = mpAnimations.find(name);
	if (iter == mpAnimations.end())
	{
		return nullptr;
	}

	return iter->second;
}

void SpriteAnimationComponent::PlayAnimation(const std::wstring& name)
{
	PlayAnimation(name, true);
}

void SpriteAnimationComponent::PlayAnimation(const std::wstring& name, const bool loop)
{
	SpriteAnimation* pAnimation = FindAnimationOrNull(name);
	if (pAnimation == nullptr)
	{
		return;
	}

	if (mpActiveAnimation != nullptr)
	{
		Events* pCurrentEvents = FindEvents(mpActiveAnimation->GetName());

		if (pCurrentEvents != nullptr)
		{
			pCurrentEvents->endEvent();
		}
	}

	Events* pNextEvents = FindEvents(name);

	if (pNextEvents != nullptr)
	{
		pNextEvents->startEvent();
	}

	mpActiveAnimation = pAnimation;
	mpActiveAnimation->Reset();
	mbLoop = loop;
}

Events* SpriteAnimationComponent::FindEvents(const std::wstring& name)
{
	auto iter = mEvents.find(name);
	if (iter == mEvents.end())
	{
		return nullptr;
	}

	return iter->second;
}

Event& SpriteAnimationComponent::GetStartEvent(const std::wstring& name)
{
	Events* pEvents = FindEvents(name);

	return pEvents->startEvent;
}

Event& SpriteAnimationComponent::GetCompleteEvent(const std::wstring& name)
{
	Events* pEvents = FindEvents(name);

	return pEvents->completeEvent;
}

Event& SpriteAnimationComponent::GetEndEvent(const std::wstring& name)
{
	Events* pEvents = FindEvents(name);

	return pEvents->endEvent;
}

bool SpriteAnimationComponent::IsCompelete() const
{
	if (mpActiveAnimation == nullptr)
	{
		return true;
	}

	return mpActiveAnimation->IsComplete();
}
