#pragma once

#include "Component.h"

class Collider2DComponent;

class ScriptComponent : public Component
{
public:
	ScriptComponent();
	virtual ~ScriptComponent() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual void Render() const = 0;

	virtual void OnCollisionEnter(Collider2DComponent* const pOther) = 0;
	virtual void OnCollisionStay(Collider2DComponent* const pOther) = 0;
	virtual void OnCollisionExit(Collider2DComponent* const pOther) = 0;

	EComponentType GetComponentType() const override
	{
		return EComponentType::SCRIPT;
	}

private:
};