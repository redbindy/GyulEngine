#pragma once

#include "Core/MathHelper.h"

enum
{
	MAX_LIGHTS = 8
};

struct Light
{
	Vector3 position;
	float falloffStart;
	Vector3 strength;
	float falloffEnd;
	Vector3 direction;
	float spotPower;
};
static_assert(sizeof(Light) % 16 == 0);