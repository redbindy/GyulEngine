#pragma once

#include "Component.h"
#include "../Core/MathHelper.h"

class Actor;

class CameraComponent final : public Component
{
public:
	CameraComponent();
	virtual ~CameraComponent() = default;

	void Initialize() override;
	void Update() override;
	void LateUpdate() override;
	void Render() const override;

	EComponentType GetComponentType() const
	{
		return EComponentType::CAMERA;
	}

	void SetTarget(Actor* const pTarget)
	{
		mpTarget = pTarget;
	}

private:
	Actor* mpTarget;

private:
	CameraComponent(const CameraComponent& other) = delete;
	CameraComponent& operator=(const CameraComponent& other) = delete;
	CameraComponent(CameraComponent&& other) noexcept = delete;
	CameraComponent& operator=(CameraComponent&& other) noexcept = delete;
};