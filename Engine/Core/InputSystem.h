#pragma once

#include <cstdint>

#include "Core/MathHelper.h"

class InputSystem final
{
public:
	void ClearDelta();

	inline void OnKeyDown(const uint8_t key)
	{
		mbKeyPressed[key] = true;
	}

	inline void OnKeyUp(const uint8_t key)
	{
		mbKeyPressed[key] = false;
	}

	inline bool IsKeyPressed(const uint8_t key) const
	{
		return mbKeyPressed[key];
	}

	inline void SetMousePosition(const uint16_t x, const uint16_t y)
	{
		mMousePrevPosition = mMousePosition;

		mMousePosition = Vector2(x, y);
	}

	inline Vector2 GetMousePosition() const
	{
		return mMousePosition;
	}

	inline Vector2 GetMouseDelta() const
	{
		return mMousePosition - mMousePrevPosition;
	}

	// static
	static void Initialize();

	static InputSystem& GetInstance()
	{
		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static InputSystem* spInstance;

	bool mbKeyPressed[UINT8_MAX];

	Vector2 mMousePrevPosition;
	Vector2 mMousePosition;

private:
	InputSystem();
	~InputSystem() = default;

private:
	InputSystem(const InputSystem& other) = delete;
	InputSystem(InputSystem&& other) = delete;
	InputSystem& operator=(const InputSystem& other) = delete;
	InputSystem& operator=(InputSystem&& other) = delete;
};