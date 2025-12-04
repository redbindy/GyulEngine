#pragma once

#include "../Resources/Resource.h"

class AudioClip final : public Resource
{
public:
	AudioClip();
	virtual ~AudioClip();

	virtual HRESULT Load(const std::wstring& path) override;

	void Play();
	void Stop();
	// void Set3DAttributes();
	void SetLoop(const bool bLoop)
	{
		mbLoop = bLoop;
	}

private:
	bool mbLoop;
};