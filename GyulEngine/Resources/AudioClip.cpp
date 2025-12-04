#include "AudioClip.h"

AudioClip::AudioClip()
	: Resource(L"AudioClip")
	, mbLoop(false)
{

}

AudioClip::~AudioClip()
{
	Stop();
}

HRESULT AudioClip::Load(const std::wstring& path)
{
	return E_NOTIMPL;
}

void AudioClip::Play()
{
	if (mbLoop)
	{

	}
	else
	{

	}
}

void AudioClip::Stop()
{
}