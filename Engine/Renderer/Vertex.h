#pragma once

#include <d3d11.h>

#include "Core/MathHelper.h"

#define VERTEX_LIST \
	VERTEX_ENTRY(POS_UV, PosUV) \
	VERTEX_ENTRY(POS_NORMAL_UV, PosNormalUV) \

namespace Vertex
{
	struct PosUV
	{
		Vector3 pos;
		Vector2 uv;
	};

	struct PosNormalUV
	{
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

	enum class EType : uint8_t
	{
#define VERTEX_ENTRY(type, name) type,
		VERTEX_LIST
#undef VERTEX_ENTRY

		COUNT
	};

	consteval int GetVertexTypeCount()
	{
		return static_cast<int>(EType::COUNT);
	}

	constexpr int GetTypeInt(const EType type)
	{
		return static_cast<int>(type);
	}

	constexpr const char* const GetTypeName(const EType type)
	{
		constexpr const char* const names[] =
		{
		#define VERTEX_ENTRY(type, name) #type,
			VERTEX_LIST
		#undef VERTEX_ENTRY
		};

		return names[GetTypeInt(type)];
	}

	constexpr std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputLayoutDescs(const EType type)
	{
		switch (type)
		{
		case EType::POS_UV:
			{
				return
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(PosUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
				};
			}
			break;

		case EType::POS_NORMAL_UV:
			{
				return
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(PosNormalUV, normal), D3D11_INPUT_PER_VERTEX_DATA, 0},
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(PosNormalUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0},
				};
			}
			break;

		default:
			ASSERT(false);
			return {};
		}
	}
};