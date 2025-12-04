#pragma once

#define RESOURCE_TYPE_LIST \
	RESOURCE_ENTRY(TEXTURE, L"Texture") \
	RESOURCE_ENTRY(AUDIO_CLIP, L"AudioClip") \
	RESOURCE_ENTRY(PREFAB, L"Prefab") \
	RESOURCE_ENTRY(SPRITE_ANIMATION, L"SpriteAnimation") \

enum class EResourceType
{
#define RESOURCE_ENTRY(type, name) type,
	RESOURCE_TYPE_LIST
#undef RESOURCE_ENTRY

	COUNT
};

consteval int GetResourceTypeInt(const EResourceType type)
{
	return static_cast<int>(type);
}

consteval int GetResourceTypeCount()
{
	return static_cast<int>(EResourceType::COUNT);
}

constexpr const wchar_t* GetResourceTypeName(const EResourceType type)
{
	const wchar_t* const names[] = {
	#define RESOURCE_ENTRY(type, name) name,
		RESOURCE_TYPE_LIST
	#undef RESOURCE_ENTRY
	};

	return names[static_cast<int>(type)];
}