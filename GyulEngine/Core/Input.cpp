#include "Input.h"

#if defined(max)
#undef max
#endif

#include <limits>

enum
{
	NUM_KEYS = std::numeric_limits<unsigned char>::max() + 1
};

Input* Input::spInstance = nullptr;

Input::Input()
	: mKeys()
{
	mKeys.reserve(NUM_KEYS);

	for (uint32_t i = 0; i < NUM_KEYS; ++i)
	{
		Key key;
		key.keyCode = i;
		key.state = EKeyState::NONE;
		key.bPressed = false;

		mKeys.push_back(key);
	}
}

void Input::Update()
{
	for (int i = 0; i < NUM_KEYS; ++i)
	{
		if (GetAsyncKeyState(i) & 0x8000)
		{
			if (mKeys[i].bPressed)
			{
				mKeys[i].state = EKeyState::PRESSED;
			}
			else
			{
				mKeys[i].state = EKeyState::DOWN;
			}

			mKeys[i].bPressed = true;
		}
		else
		{
			if (mKeys[i].bPressed)
			{
				mKeys[i].state = EKeyState::UP;
			}
			else
			{
				mKeys[i].state = EKeyState::NONE;
			}
		}
	}
}

void Input::CreateInstance()
{
	ASSERT(spInstance == nullptr);

	spInstance = new Input();
}
