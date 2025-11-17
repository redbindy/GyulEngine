#pragma once

#include <vector>

#include "Renderer/Vertex.h"

class Shape final
{
public:
	static inline void CreateTriangleDataAlloc(
		std::vector<Vertex::PosNormalUV>& outVertices,
		std::vector<uint16_t>& outIndices
	)
	{
		outVertices = {
			{ { 0.f, 1.0f, 0.f }, { 0.f, 0.f, -1.f }, { 0.5f, 0.f } },
			{ { 1.f, -1.f, 0.f }, { 0.f, 0.f, -1.f }, { 1.f, 1.f } },
			{ { -1.f, -1.f, 0.f }, { 0.f, 0.f, -1.f }, { 0.f, 1.f } }
		};

		outIndices = {
			0, 1, 2
		};
	}

	static inline void CreateSquareDataAlloc(
		std::vector<Vertex::PosNormalUV>& outVertices,
		std::vector<uint16_t>& outIndices
	)
	{
		outVertices = {
			{ { -1, -1, 0.f }, { 0.f, 0.f, -1.f }, { 0.f, 1.f } },
			{ { -1, 1, 0.f }, { 0.f, 0.f, -1.f }, { 0.f, 0.f } },
			{ { 1, -1, 0.f }, { 0.f, 0.f, -1.f }, { 1.f, 1.f } },
			{ { 1, 1, 0.f }, { 0.f, 0.f, -1.f }, { 1.f, 0.f } }
		};

		outIndices = {
			0, 1, 2,
			2, 1, 3
		};
	}

	static inline void CreateCubeDataAlloc(
		std::vector<Vertex::PosNormalUV>& outVertices,
		std::vector<uint16_t>& outIndices
	)
	{
		outVertices = {
			// À­¸é
			{ { -1.f, 1.f, -1.f }, { 0.f, 1.f, 0.f }, { 0.f, 0.f } },
			{ { -1.f, 1.f, 1.f }, { 0.f, 1.f, 0.f }, { 1.f, 0.f } },
			{ { 1.f, 1.f, 1.f }, { 0.f, 1.f, 0.f }, { 1.f, 1.f } },
			{ { 1.f, 1.f, -1.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f } },

			// ¾Æ·§¸é
			{ { -1.f, -1.f, -1.f }, { 0.f, -1.f, 0.f }, { 0.f, 0.f } },
			{ { 1.f, -1.f, -1.f }, { 0.f, -1.f, 0.f }, { 1.f, 0.f } },
			{ { 1.f, -1.f, 1.f }, { 0.f, -1.f, 0.f }, { 1.f, 1.f } },
			{ { -1.f, -1.f, 1.f }, { 0.f, -1.f, 0.f }, { 0.f, 1.f } },

			// ¾Õ¸é
			{ { -1.f, -1.f, -1.f }, { 0.f, 0.f, -1.f }, { 0.f, 0.f } },
			{ { -1.f, 1.f, -1.f }, { 0.f, 0.f, -1.f }, { 1.f, 0.f } },
			{ { 1.f, 1.f, -1.f }, { 0.f, 0.f, -1.f }, { 1.f, 1.f } },
			{ { 1.f, -1.f, -1.f }, { 0.f, 0.f, -1.f }, { 0.f, 1.f } },

			// µÞ¸é
			{ { -1.f, -1.f, 1.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f } },
			{ { 1.f, -1.f, 1.f }, { 0.f, 0.f, 1.f }, { 1.f, 0.f } },
			{ { 1.f, 1.f, 1.f }, { 0.f, 0.f, 1.f }, { 1.f, 1.f } },
			{ { -1.f, 1.f, 1.f }, { 0.f, 0.f, 1.f }, { 0.f, 1.f } },

			// ¿ÞÂÊ
			{ { -1.f, -1.f, 1.f }, { -1.f, 1.f, 0.f }, { 0.f, 0.f } },
			{ { -1.f, 1.f, 1.f }, { -1.f, 1.f, 0.f }, { 1.f, 0.f } },
			{ { -1.f, 1.f, -1.f }, { -1.f, 1.f, 0.f }, { 1.f, 1.f } },
			{ { -1.f, -1.f, -1.f }, { -1.f, 1.f, 0.f }, { 0.f, 1.f } },

			// ¿À¸¥ÂÊ
			{ { 1.f, -1.f, 1.f }, { 1.f, 0.f, 0.f }, { 0.f, 0.f } },
			{ { 1.f, -1.f, -1.f }, { 1.f, 0.f, 0.f }, { 1.f, 0.f } },
			{ { 1.f, 1.f, -1.f }, { 1.f, 0.f, 0.f }, { 1.f, 1.f } },
			{ { 1.f, 1.f, 1.f }, { 1.f, 0.f, 0.f }, { 0.f, 1.f } }
		};

		outIndices = {
			// À­¸é
			0,  1,  2,
			0,  2,  3,

			// ¾Æ·§¸é
			4,  5,  6,
			4,  6,  7,

			// ¾Õ¸é
			8,  9,  10,
			8,  10, 11,

			// µÞ¸é
			12, 13, 14,
			12, 14, 15,

			// ¿ÞÂÊ
			16, 17, 18,
			16, 18, 19,

			// ¿À¸¥ÂÊ
			20, 21, 22,
			20, 22, 23
		};
	}

	static inline void CreateSphereDataAlloc(
		std::vector<Vertex::PosNormalUV>& outVertices,
		std::vector<uint16_t>& outIndices
	)
	{
		constexpr float radius = 1.f;
		constexpr int32_t sliceCount = 20;
		constexpr int32_t stackCount = 20;

		std::vector<Vertex::PosNormalUV> vertices;
		vertices.reserve((stackCount + 1) * (sliceCount + 1));

		std::vector<uint16_t> indices;
		indices.reserve(vertices.capacity() * 6);

		constexpr float DELTA_THETA = -XM_2PI / sliceCount;
		constexpr float DELTA_PHI = -XM_PI / stackCount;

		const Vector3 startPoint(0.f, -radius, 0.f);
		for (int y = 0; y <= stackCount; ++y)
		{
			const Matrix rotationZ = Matrix::CreateRotationZ(DELTA_PHI * y);
			const Vector3 sliceStart = Vector3::Transform(startPoint, rotationZ);

			for (int x = 0; x <= sliceCount; ++x)
			{
				const Matrix rotationY = Matrix::CreateRotationY(DELTA_THETA * x);

				const Vector3 currPoint = Vector3::Transform(sliceStart, rotationY);
				Vector3 normal = currPoint; // from ¿øÁ¡ to Á¤Á¡ º¤ÅÍ·Î °¡Á¤
				normal.Normalize();

				const Vector2 uv = Vector2(x / static_cast<float>(sliceCount), y / static_cast<float>(stackCount));

				const Vertex::PosNormalUV vertex = { currPoint, normal, uv };

				vertices.push_back(vertex);
			}
		}

		for (int y = 0; y < stackCount; ++y)
		{
			const int start = (sliceCount + 1) * y;
			for (int x = 0; x < sliceCount; ++x)
			{
				indices.push_back(start + x);
				indices.push_back(start + x + sliceCount + 1);
				indices.push_back(start + x + 1 + sliceCount + 1);

				indices.push_back(start + x);
				indices.push_back(start + x + 1 + sliceCount + 1);
				indices.push_back(start + x + 1);
			}
		}

		outVertices = std::move(vertices);
		outIndices = std::move(indices);
	}
};