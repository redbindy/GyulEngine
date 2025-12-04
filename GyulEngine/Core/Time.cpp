#include "Time.h"

Time* Time::spInstance = nullptr;

Time::Time()
	: mFrequency{}
	, mPrevTicks{}
	, mCurrTicks{}
	, mDeltaTime(0.0f)
{
	QueryPerformanceFrequency(&mFrequency);
	QueryPerformanceCounter(&mPrevTicks);
}

void Time::Update()
{
	QueryPerformanceCounter(&mCurrTicks);

	const LONGLONG deltaTicks = mCurrTicks.QuadPart - mPrevTicks.QuadPart;

	mDeltaTime = deltaTicks / static_cast<float>(mFrequency.QuadPart);

	mPrevTicks = mCurrTicks;
}

void Time::CreateInstance()
{
	ASSERT(spInstance == nullptr);

	spInstance = new Time();
}