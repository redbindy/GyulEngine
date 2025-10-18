#pragma once

#include <d3d11.h>

#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class DebugSphere final
{
public:
	DebugSphere();
	~DebugSphere();
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

	ID3D11Buffer* mpVertexBuffer;
	ID3D11Buffer* mpIndexBuffer;
	int mIndexCount;

	ID3D11InputLayout* mpInputLayout;
	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveTopology;

	ID3D11VertexShader* mpVertexShader;
	ID3D11PixelShader* mpPixelShader;

	ID3D11BlendState* mpBlendState;
};