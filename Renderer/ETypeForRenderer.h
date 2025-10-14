#pragma once

#include <cstdint>

#include "DebugHelper.h"

enum class EShaderType : uint8_t
{
	VERTEX = 0,
	PIXEL,
	COUNT
};

enum class EBufferType : uint8_t
{
	VERTEX = 0,
	INDEX,
	CONSTANT,
	COUNT
};

enum class ESamplerType : uint8_t
{
	WRAP_LINEAR = 0,
	COUNT
};

constexpr const char* const GetSamplerTypeString(const ESamplerType type)
{
	ASSERT(static_cast<uint8_t>(type) < static_cast<uint8_t>(ESamplerType::COUNT));

	constexpr const char* const TYPE_STRING[static_cast<uint8_t>(ESamplerType::COUNT)] = {
		"WrapLinear"
	};

	return TYPE_STRING[static_cast<uint8_t>(type)];
}