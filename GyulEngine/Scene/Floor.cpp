#include "Floor.h"

Floor::Floor()
	: Actor(L"Floor")
{
}

void Floor::Initialize()
{
	Actor::Initialize();
}

void Floor::Update()
{
	Actor::Update();
}

void Floor::LateUpdate()
{
	Actor::LateUpdate();
}

void Floor::Render() const
{
	Actor::Render();
}
