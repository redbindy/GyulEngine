#pragma once
#pragma once

#include "Component.h"

#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class CameraControllerComponent final : public Component
{
public:
	CameraControllerComponent(Actor* const pOwner);
	virtual ~CameraControllerComponent() = default;
	CameraControllerComponent(const CameraControllerComponent& other) = delete;
	CameraControllerComponent& operator=(const CameraControllerComponent& other) = delete;
	CameraControllerComponent(CameraControllerComponent&& other) = delete;
	CameraControllerComponent& operator=(CameraControllerComponent&& other) = delete;

	virtual void Update(const float deltaTime) override;
	virtual void DrawUI() override;

private:
	enum class EMoveKey : uint8_t
	{
		FRONT,
		BACK,
		LEFT,
		RIGHT,
		COUNT
	};

private:
	Vector2 mPrevMousePosition;

	float mSpeed;

	uint8_t mMoveKeys[static_cast<uint8_t>(EMoveKey::COUNT)];
};