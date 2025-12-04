#pragma once

#include <Windows.h>

#include "Assert.h"

class Time final
{
public:
	void Update();

	float GetDeltaTime() const
	{
		return mDeltaTime;
	}

	// static 
	static void CreateInstance();

	static Time& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static Time* spInstance;

	LARGE_INTEGER mFrequency;
	LARGE_INTEGER mPrevTicks;
	LARGE_INTEGER mCurrTicks;

	float mDeltaTime;

private:
	Time();
	virtual ~Time() = default;

private:
	Time(const Time& other) = delete;
	Time& operator=(const Time& other) = delete;
	Time(Time&& other) noexcept = delete;
	Time& operator=(Time&& other) noexcept = delete;
};