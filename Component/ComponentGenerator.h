#pragma once

#include "DebugHelper.h"

#include "Component.h"
#include "MeshComponent.h"
#include "CameraComponent.h"
#include "CameraControllerComponent.h"

#include <cstdint>

class Actor;

namespace ComponentGenerator
{
	enum class EComponentType : uint8_t
	{
		MESH = 0,
		CAMERA,
		CAMERA_CONTROLLER,
		COUNT
	};

	inline consteval uint8_t GetComponentCount()
	{
		return static_cast<uint8_t>(EComponentType::COUNT);
	}

	constexpr const char* const COMPONENT_NAMES[GetComponentCount()] = {
		"Mesh",
		"Camera",
		"CameraController"
	};

	static std::function<Component* (Actor* const)> sConstructors[GetComponentCount()] = {
		[](Actor* const pActor) { return new MeshComponent(pActor); },
		[](Actor* const pActor) { return new CameraComponent(pActor); },
		[](Actor* const pActor) { return new CameraControllerComponent(pActor); }
	};
};