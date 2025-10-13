#pragma once

#include "SimpleMath.h"
#include "DebugHelper.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

enum class EVertexType : uint8_t
{
	POS_NORMAL_UV = 0,
	COUNT
};

struct Vertex
{
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;
};
static_assert(sizeof(Vertex) % 4 == 0);

constexpr inline const char* const GetVertexTypeString(const EVertexType type)
{
	ASSERT(static_cast<uint8_t>(type) < static_cast<uint8_t>(EVertexType::COUNT));

	constexpr const char* const TYPE_STRING[static_cast<uint8_t>(EVertexType::COUNT)] = {
		"PosNormalUV"
	};

	return TYPE_STRING[static_cast<uint8_t>(type)];
}