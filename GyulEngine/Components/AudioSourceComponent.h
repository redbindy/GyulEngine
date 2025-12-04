#pragma once

#include "Component.h"

class AudioClip;

class AudioSourceComponent final : public Component
{
public:
	AudioSourceComponent();
	virtual ~AudioSourceComponent() = default;

	void Initialize() override;
	void Update() override;
	void LateUpdate() override;
	void Render() const override;

	void Play();
	void Stop();
	void SetLoop(const bool bLoop);

	EComponentType GetComponentType() const override
	{
		return EComponentType::AUDIO_SOURCE;
	}

	void SetClip(AudioClip* pClip)
	{
		mpClip = pClip;
	}

	AudioClip* GetClip() const
	{
		return mpClip;
	}

private:
	AudioClip* mpClip;
};