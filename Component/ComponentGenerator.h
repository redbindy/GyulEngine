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

#define COMPONENT_LIST \
	COMPONENT_ENTRY(Mesh) \
	COMPONENT_ENTRY(Camera) \
	COMPONENT_ENTRY(CameraController) \
	COMPONENT_ENTRY(Light) \
	COMPONENT_ENTRY(Sprite) \
	COMPONENT_ENTRY(Controller2D) \

namespace ComponentGenerator
{
	enum class EComponentType : uint8_t
	{
#define COMPONENT_ENTRY(name) name, 
		COMPONENT_LIST
#undef COMPONENT_ENTRY
		COUNT
	};

	inline consteval uint8_t GetComponentCount()
	{
		return static_cast<uint8_t>(EComponentType::COUNT);
	}

	constexpr const char* const COMPONENT_NAMES[GetComponentCount()] = {
#define COMPONENT_ENTRY(name) (#name), 
		COMPONENT_LIST
#undef COMPONENT_ENTRY
	};

	static std::function<Component* (Actor* const)> sConstructors[GetComponentCount()] = {
#define COMPONENT_ENTRY(name) ([](Actor* const pActor) { return new name##Component(pActor); }), 
		COMPONENT_LIST
#undef COMPONENT_ENTRY
	};

	inline const char* GetNameString(const EComponentType type)
	{
		return COMPONENT_NAMES[static_cast<uint8_t>(type)];
	}
};