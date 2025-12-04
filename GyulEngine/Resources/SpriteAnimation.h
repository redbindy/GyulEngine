#pragma once

#include <vector>

#include "../Core/MathHelper.h"
#include "Resource.h"

class Texture;
class SpriteAnimationComponent;

class SpriteAnimation final : public Resource
{
public:
	struct Sprite
	{
		Vector2 leftTop;
		Vector2 size;
		Vector2 offset;
		float duration;
	};

public:
	SpriteAnimation();
	virtual ~SpriteAnimation() = default;

	HRESULT Load(const std::wstring& path) override;

	void Update();
	void Render();

	void CreateAnimation(
		const std::wstring& name,
		Texture* const pSpriteSheet,
		const Vector2 leftTop,
		const Vector2 size,
		const Vector2 offset,
		const int spriteLength,
		const float duration
	);

	void Reset();

	EResourceType GetResourceType() const override
	{
		return EResourceType::SPRITE_ANIMATION;
	}

	bool IsComplete()
	{
		return mbComplete;
	}

	void SetSpriteAnimationComponent(SpriteAnimationComponent* pComponent)
	{
		mpSpriteAnimationComponent = pComponent;
	}

private:
	SpriteAnimationComponent* mpSpriteAnimationComponent;
	Texture* mpTexture;

	std::vector<Sprite> mAnimationSheets;

	int mIndex;
	float mTime;
	bool mbComplete;
};