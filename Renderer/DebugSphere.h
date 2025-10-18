#pragma once

#include <d3d11.h>

#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Mesh;

class DebugSphere final
{
public:
	DebugSphere();
	~DebugSphere() = default;
	DebugSphere(const DebugSphere& other) = delete;
	DebugSphere& operator=(const DebugSphere& other) = delete;
	DebugSphere(DebugSphere&& other) = delete;
	DebugSphere& operator=(DebugSphere&& other) = delete;

	void Draw(ID3D11DeviceContext& deviceContext);

	void SetCenter(const Vector3 center)
	{
		mCenter = center;
	}

	void SetScale(const float scale)
	{
		mScale = scale;
	}

private:
	Vector3 mCenter;
	float mScale;

	Mesh* mpMesh;

	ID3D11VertexShader* mpVertexShader;

	ID3D11RasterizerState* mpWireframeState;

	ID3D11PixelShader* mpPixelShader;

	ID3D11BlendState* mpBlendState;
	ID3D11DepthStencilState* mpDepthReadOnlyState;
};