#pragma once

#include "DebugHelper.h"

#include "Component.h"
#include "MeshComponent.h"
#include "CameraComponent.h"
#include "CameraControllerComponent.h"
#include "LightComponent.h"
#include "SpriteComponent.h"
#include "Controller2DComponent.h"

#include <cstdint>

class Actor;

namespace ComponentGenerator
{
	enum class EComponentType : uint8_t
	{
		MESH = 0,
		CAMERA,
		CAMERA_CONTROLLER,
		LIGHT,
		SPRITE,
		CONTROLLER_2D,
		COUNT
	};

	inline consteval uint8_t GetComponentCount()
	{
		return static_cast<uint8_t>(EComponentType::COUNT);
	}

	constexpr const char* const COMPONENT_NAMES[GetComponentCount()] = {
		"Mesh",
		"Camera",
		"CameraController",
		"Light",
		"Sprite",
		"Controller2D"
	};

	static std::function<Component* (Actor* const)> sConstructors[GetComponentCount()] = {
		[](Actor* const pActor) { return new MeshComponent(pActor); },
		[](Actor* const pActor) { return new CameraComponent(pActor); },
		[](Actor* const pActor) { return new CameraControllerComponent(pActor); },
		[](Actor* const pActor) { return new LightComponent(pActor); },
		[](Actor* const pActor) { return new SpriteComponent(pActor); },
		[](Actor* const pActor) { return new Controller2DComponent(pActor); }
	};

	inline const char* GetNameString(const EComponentType type)
	{
		return COMPONENT_NAMES[static_cast<uint8_t>(type)];
	}
};