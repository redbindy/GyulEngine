#pragma once

#include <cstdint>

#if defined(OPAQUE)
#undef OPAQUE
#endif

// rs
enum class ERasterizerType : uint8_t
{
	SOLID,
	WIREFRAME,
	COUNT
};

consteval int GetRasterizerTypeCount()
{
	return static_cast<int>(ERasterizerType::COUNT);
}

consteval int GetRasterizerTypeInt(const ERasterizerType type)
{
	return static_cast<int>(type);
}

// texture
enum class ESamplerType : uint8_t
{
	LINEAR_WRAP,
	COUNT
};

consteval int GetSamplerTypeCount()
{
	return static_cast<int>(ESamplerType::COUNT);
}

consteval int GetSamplerTypeInt(const ESamplerType type)
{
	return static_cast<int>(type);
}

// om
enum class EBlendStateType : uint8_t
{
	OPAQUE,
	ALPHA_BLEND,
	ADDITIVE,
	COUNT
};

consteval int GetBlendTypeCount()
{
	return static_cast<int>(EBlendStateType::COUNT);
}

consteval int GetBlendTypeInt(const EBlendStateType type)
{
	return static_cast<int>(type);
}

enum class EDepthStencilType : uint8_t
{
	DEPTH_ENABLED,
	DEPTH_DISABLED,
	COUNT
};

consteval int GetDepthStencilTypeCount()
{
	return static_cast<int>(EDepthStencilType::COUNT);
}

consteval int GetDepthStencilTypeInt(const EDepthStencilType type)
{
	return static_cast<int>(type);
}
