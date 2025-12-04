#pragma once

#define COMPONENT_TYPE_LIST \
	COMPONENT_TYPE_ENTRY(TRANSFORM, L"Transform") \
	COMPONENT_TYPE_ENTRY(SPRITE, L"Sprite") \
	COMPONENT_TYPE_ENTRY(CAMERA, L"Camera") \
	COMPONENT_TYPE_ENTRY(SCRIPT, L"Script") \
	COMPONENT_TYPE_ENTRY(SPRITE_ANIMATION, L"SpriteAnimation") \
	COMPONENT_TYPE_ENTRY(COLLIDER, L"Collider") \
	COMPONENT_TYPE_ENTRY(RIGIDBODY, L"Rigidbody") \
	COMPONENT_TYPE_ENTRY(AUDIO_LISTENER, L"AudioListener") \
	COMPONENT_TYPE_ENTRY(AUDIO_SOURCE, L"AudioSource")

enum class EComponentType
{
#define COMPONENT_TYPE_ENTRY(type, name) type,
	COMPONENT_TYPE_LIST
#undef COMPONENT_TYPE_ENTRY

	COUNT
};

consteval int GetComponentTypeInt(const EComponentType type)
{
	return static_cast<int>(type);
}

consteval int GetComponentTypeCount()
{
	return static_cast<int>(EComponentType::COUNT);
}

constexpr const wchar_t* GetComponentTypeName(const EComponentType type)
{
	const wchar_t* const names[] = {
	#define COMPONENT_TYPE_ENTRY(type, name) name,
		COMPONENT_TYPE_LIST
	#undef COMPONENT_TYPE_ENTRY
	};

	return names[static_cast<int>(type)];
}