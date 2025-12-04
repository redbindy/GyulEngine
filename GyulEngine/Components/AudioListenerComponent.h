#pragma once

#include "Component.h"

class AudioListenerComponent final : public Component
{
public:
	AudioListenerComponent();
	virtual ~AudioListenerComponent() = default;

	void Initialize() override;
	void Update() override;
	void LateUpdate() override;
	void Render() const override;

	EComponentType GetComponentType() const override
	{
		return EComponentType::AUDIO_LISTENER;
	}

private:
};