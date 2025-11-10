#include "CameraComponent.h"

#include "Core/MathHelper.h"
#include "../Actor.h"
#include "Renderer/Renderer.h"

CameraComponent::CameraComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder)
	: Component(pOwner, label, updateOrder)
	, mbOrhographic(false)
	, mViewWidth(100.f)
	, mViewHeight(100.f)
	, mNearZ(0.1f)
	, mFarZ(100.f)
	, mFov(XMConvertToRadians(45.f))
{
}

void CameraComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	Actor& owner = GetOwner();

	const Vector3 position = owner.GetPosition();

	const Vector3 front = Vector3::Transform(Vector3::UnitZ, owner.GetRotation());
	const Vector3 up = Vector3::Transform(Vector3::UnitY, owner.GetRotation());

	const Matrix view = XMMatrixLookToLH(position, front, up);

	Renderer& renderer = Renderer::GetInstance();

	Matrix proj;
	if (mbOrhographic)
	{
		proj = XMMatrixOrthographicLH(mViewWidth, mViewHeight, mNearZ, mFarZ);
	}
	else
	{
		const D3D11_VIEWPORT& viewport = renderer.GetViewport();

		const float aspectRatio = viewport.Width / viewport.Height;

		proj = XMMatrixPerspectiveFovLH(mFov, aspectRatio, mNearZ, mFarZ);
	}

	const Matrix viewProj = view * proj;

	renderer.UpdateCBFrame(position, viewProj);
}
