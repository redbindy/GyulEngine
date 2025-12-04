#pragma once

#include <vector>
#include <cstdint>

#include "Assert.h"

class Input final
{
public:
	enum class EKeyState
	{
		NONE,
		PRESSED,
		DOWN,
		UP,
	};

	struct Key
	{
		uint32_t keyCode;
		EKeyState state;
		bool bPressed;
	};

public:
	void Update();

	bool IsKeyDown(const uint32_t keyCode) const
	{
		return mKeys[keyCode].state == EKeyState::DOWN;
	}

	bool IsKeyUp(const uint32_t keyCode) const
	{
		return mKeys[keyCode].state == EKeyState::UP;
	}

	bool IsKeyPressed(const uint32_t keyCode) const
	{
		return mKeys[keyCode].state == EKeyState::PRESSED;
	}

	// static
	static void CreateInstance();

	static Input& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static Input* spInstance;

	std::vector<Key> mKeys;

private:
	Input();
	virtual ~Input() = default;

private:
	Input(const Input& other) = delete;
	Input& operator=(const Input& other) = delete;
	Input(Input&& other) noexcept = delete;
	Input& operator=(Input&& other) noexcept = delete;
};