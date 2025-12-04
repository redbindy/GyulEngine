#pragma once

#define LAYER_TYPE_LIST \
	LAYER_TYPE_ENTRY(NONE, L"None") \
	LAYER_TYPE_ENTRY(BACKGROUND, L"Background") \
	LAYER_TYPE_ENTRY(PARTICLE, L"Particle") \
	LAYER_TYPE_ENTRY(TILE, L"Tile") \

enum class ELayerType
{
#define LAYER_TYPE_ENTRY(type, name) type, 
	LAYER_TYPE_LIST
#undef LAYER_TYPE_ENTRY

	COUNT
};

consteval int GetLayerTypeInt(const ELayerType type)
{
	return static_cast<int>(type);
}

consteval int GetLayerTypeCount()
{
	return static_cast<int>(ELayerType::COUNT);
}

constexpr const wchar_t* GetLayerTypeName(const ELayerType type)
{
	const wchar_t* const names[] = {
	#define LAYER_TYPE_ENTRY(type, name) name,
		LAYER_TYPE_LIST
	#undef LAYER_TYPE_ENTRY
	};

	return names[static_cast<int>(type)];
}