#pragma once

#include <cstdint>

#if defined(OPAQUE)
#undef OPAQUE
#endif

// rs
#define RASTERIZER_STATE_LIST \
	RSTERIZER_STATE_ENTRY(SOLID, Solid) \
	RSTERIZER_STATE_ENTRY(WIREFRAME, WireFrame) \

enum class ERasterizerType : uint8_t
{
#define RSTERIZER_STATE_ENTRY(type, name) type,
	RASTERIZER_STATE_LIST
#undef RSTERIZER_STATE_ENTRY

	COUNT
};

consteval int GetRasterizerTypeCount()
{
	return static_cast<int>(ERasterizerType::COUNT);
}

constexpr int GetRasterizerTypeInt(const ERasterizerType type)
{
	return static_cast<int>(type);
}

constexpr const char* const GetRasterizerTypeName(const ERasterizerType type)
{
	constexpr const char* const names[] =
	{
	#define RSTERIZER_STATE_ENTRY(type, name) #name,
		RASTERIZER_STATE_LIST
	#undef RSTERIZER_STATE_ENTRY
	};

	return names[GetRasterizerTypeInt(type)];
}

// sampler
#define SAMPLER_STATE_LIST \
	SAMPLER_STATE_ENTRY(LINEAR_WRAP, LinearWrap) \

enum class ESamplerType : uint8_t
{
#define SAMPLER_STATE_ENTRY(type, name) type,
	SAMPLER_STATE_LIST
#undef SAMPLER_STATE_ENTRY

	COUNT
};

consteval int GetSamplerTypeCount()
{
	return static_cast<int>(ESamplerType::COUNT);
}

constexpr int GetSamplerTypeInt(const ESamplerType type)
{
	return static_cast<int>(type);
}

constexpr const char* const GetSamplerTypeName(const ESamplerType type)
{
	constexpr const char* const names[] =
	{
	#define SAMPLER_STATE_ENTRY(type, name) #name,
		SAMPLER_STATE_LIST
	#undef SAMPLER_STATE_ENTRY
	};

	return names[GetSamplerTypeInt(type)];
}

// om
#define BLEND_STATE_LIST \
	BLEND_STATE_ENTRY(OPAQUE, Opaque) \
	BLEND_STATE_ENTRY(ALPHA_BLEND, AlphaBlend) \
	BLEND_STATE_ENTRY(ADDITIVE, Additive) \

enum class EBlendStateType : uint8_t
{
#define BLEND_STATE_ENTRY(type, name) type,
	BLEND_STATE_LIST
#undef BLEND_STATE_ENTRY

	COUNT
};

consteval int GetBlendTypeCount()
{
	return static_cast<int>(EBlendStateType::COUNT);
}

constexpr int GetBlendTypeInt(const EBlendStateType type)
{
	return static_cast<int>(type);
}

constexpr const char* const GetBlendTypeName(const EBlendStateType type)
{
	constexpr const char* const names[] =
	{
	#define BLEND_STATE_ENTRY(type, name) #name,
		BLEND_STATE_LIST
	#undef BLEND_STATE_ENTRY
	};

	return names[GetBlendTypeInt(type)];
}

#define DEPTH_STENCIL_STATE_LIST \
	DEPTH_STENCIL_STATE_ENTRY(DEPTH_ENABLED, DepthEnabled) \
	DEPTH_STENCIL_STATE_ENTRY(DEPTH_DISABLED, DepthDisabled) \

enum class EDepthStencilType : uint8_t
{
#define DEPTH_STENCIL_STATE_ENTRY(type, name) type,
	DEPTH_STENCIL_STATE_LIST
#undef DEPTH_STENCIL_STATE_ENTRY

	COUNT
};

consteval int GetDepthStencilTypeCount()
{
	return static_cast<int>(EDepthStencilType::COUNT);
}

constexpr int GetDepthStencilTypeInt(const EDepthStencilType type)
{
	return static_cast<int>(type);
}

constexpr const char* const GetDepthStencilTypeName(const EDepthStencilType type)
{
	constexpr const char* const names[] =
	{
	#define DEPTH_STENCIL_STATE_ENTRY(type, name) #name,
		DEPTH_STENCIL_STATE_LIST
	#undef DEPTH_STENCIL_STATE_ENTRY
	};

	return names[GetDepthStencilTypeInt(type)];
}
