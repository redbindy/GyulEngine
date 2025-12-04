#include "SpriteAnimation.h"

#include "../Core/Time.h"
#include "../Scene/Components/SpriteAnimationComponent.h"

SpriteAnimation::SpriteAnimation()
	: Resource(L"SpriteAnimation")
	, mpSpriteAnimationComponent(nullptr)
	, mpTexture(nullptr)
	, mAnimationSheets()
	, mIndex(-1)
	, mTime(0.0f)
	, mbComplete(false)
{

}

HRESULT SpriteAnimation::Load(const std::wstring& path)
{
	return E_NOTIMPL;
}

void SpriteAnimation::Update()
{
	if (mbComplete)
	{
		return;
	}

	Time& time = Time::GetInstance();

	mTime += time.GetDeltaTime();

	if (mAnimationSheets[mIndex].duration <= mTime)
	{
		mTime = 0.f;

		if (mIndex < mAnimationSheets.size() - 1)
		{
			++mIndex;
		}
		else
		{
			mbComplete = true;
		}
	}
}

void SpriteAnimation::Render()
{
	if (mpTexture == nullptr)
	{
		return;
	}
}

void SpriteAnimation::CreateAnimation(
	const std::wstring& name,
	Texture* const pSpriteSheet,
	const Vector2 leftTop,
	const Vector2 size,
	const Vector2 offset,
	const int spriteLength,
	const float duration
)
{
	mpTexture = pSpriteSheet;

	for (int i = 0; i < spriteLength; ++i)
	{
		Sprite sprite;
		ZeroMemory(&sprite, sizeof(Sprite));

		sprite.leftTop.x = leftTop.x + (size.x * i);
		sprite.leftTop.y = leftTop.y;
		sprite.size = size;
		sprite.offset = offset;
		sprite.duration = duration;

		mAnimationSheets.push_back(sprite);
	}
}

void SpriteAnimation::Reset()
{
	mTime = 0.f;
	mIndex = 0;
	mbComplete = false;
}
