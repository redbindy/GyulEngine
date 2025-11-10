#include "InputSystem.h"

#include "Core/Assert.h"

InputSystem* InputSystem::spInstance = nullptr;

InputSystem::InputSystem()
	: mbKeyPressed{ false, }
	, mMousePrevPosition(0.f, 0.f)
	, mMousePosition(0.f, 0.f)
{

}

void InputSystem::ClearDelta()
{
	mMousePrevPosition = mMousePosition;
}

void InputSystem::Initialize()
{
	ASSERT(spInstance == nullptr);

	spInstance = new InputSystem();
}