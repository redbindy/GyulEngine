#pragma once

#include <unordered_map>

#include "Component.h"
#include "../Core/MathHelper.h"

class Texture;
class SpriteAnimation;
class Event;
struct Events;

class SpriteAnimationComponent final : public Component
{
public:
	SpriteAnimationComponent();
	virtual ~SpriteAnimationComponent();

	void Initialize() override;
	void Update() override;
	void LateUpdate() override;
	void Render() const override;

	void CreateAnimation(
		const std::wstring& name,
		Texture* const pSpriteSheet,
		const Vector2 leftTop,
		const Vector2 size,
		const Vector2 offset,
		const int spriteLength,
		const float duration
	);

	void CreateAnimationByFolder(
		const std::wstring& name,
		const std::wstring& path,
		const Vector2 offset,
		const float duration
	);

	SpriteAnimation* FindAnimationOrNull(const std::wstring& name) const;

	void PlayAnimation(const std::wstring& name);
	void PlayAnimation(const std::wstring& name, const bool loop);

	Events* FindEvents(const std::wstring& name);

	Event& GetStartEvent(const std::wstring& name);
	Event& GetCompleteEvent(const std::wstring& name);
	Event& GetEndEvent(const std::wstring& name);

	bool IsCompelete() const;

	EComponentType GetComponentType() const override
	{
		return EComponentType::SPRITE_ANIMATION;
	}

private:
	std::unordered_map<std::wstring, SpriteAnimation*> mpAnimations;
	SpriteAnimation* mpActiveAnimation;
	bool mbLoop;

	std::unordered_map<std::wstring, Events*> mEvents;

private:
	SpriteAnimationComponent(const SpriteAnimationComponent& other) = delete;
	SpriteAnimationComponent& operator=(const SpriteAnimationComponent& other) = delete;
	SpriteAnimationComponent(SpriteAnimationComponent&& other) noexcept = delete;
	SpriteAnimationComponent& operator=(SpriteAnimationComponent&& other) noexcept = delete;
};