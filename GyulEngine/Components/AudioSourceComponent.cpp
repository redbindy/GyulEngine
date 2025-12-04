#include "AudioSourceComponent.h"

#include "../Resources/AudioClip.h"

AudioSourceComponent::AudioSourceComponent()
	: Component(L"AudioSource")
	, mpClip(nullptr)
{
}

void AudioSourceComponent::Initialize()
{
}

void AudioSourceComponent::Update()
{
}

void AudioSourceComponent::LateUpdate()
{
}

void AudioSourceComponent::Render() const
{
}

void AudioSourceComponent::Play()
{
	if (mpClip != nullptr)
	{
		mpClip->Play();
	}
}

void AudioSourceComponent::Stop()
{
	if (mpClip != nullptr)
	{
		mpClip->Stop();
	}
}

void AudioSourceComponent::SetLoop(const bool bLoop)
{
	if (mpClip != nullptr)
	{
		mpClip->SetLoop(bLoop);
	}
}

