#pragma once

#include <functional>

class Event final
{
public:
	Event() = default;
	virtual ~Event() = default;

	void operator=(std::function<void()> eventFunc)
	{
		mEventFunc = std::move(eventFunc);
	}

	void operator()() const
	{
		if (mEventFunc != nullptr)
		{
			mEventFunc();
		}
	}

private:
	std::function<void()> mEventFunc;

private:
	Event(const Event& other) = delete;
	Event& operator=(const Event& other) = delete;
	Event(Event&& other) noexcept = delete;
	Event& operator=(Event&& other) noexcept = delete;
};

struct Events
{
	Event startEvent;
	Event completeEvent;
	Event endEvent;
};