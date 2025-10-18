#pragma once

#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class IPickingListener
{
public:
	virtual bool CheckCollision(const Ray& ray, float& outDist) = 0;
	virtual void OnCollision() = 0;
};