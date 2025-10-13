#include "CameraComponent.h"

#include "Renderer.h"
#include "Actor.h"

CameraComponent::CameraComponent(Actor* const pOwner)
	: Component(pOwner, "CameraComponent")
	, mbMain(false)
	, mbOrthogonoal(false)
	, mNearPlane(0.1f)
	, mFarPlane(100.f)
	, mFov(XMConvertToRadians(105.f))
{

}

void CameraComponent::Update(const float deltaTime)
{
	if (!mbMain)
	{
		return;
	}

	Actor* const pOwner = GetOwner();

	CBFrame buffer;

	const Vector3 position = pOwner->GetPosition();
	buffer.cameraPos = position;

	const Vector3 rotation = pOwner->GetRotation();

	const Matrix yaw = Matrix::CreateRotationY(rotation.y);

	Vector3 front(0.f, 0.f, 1.f);
	front = Vector3::Transform(front, yaw);

	Vector3 up(0.f, 1.f, 0.f);

	Vector3 right = front.Cross(up);
	right.Normalize();

	const Quaternion pitchQuat = Quaternion::CreateFromAxisAngle(right, rotation.x);
	const Matrix pitch = Matrix::CreateFromQuaternion(pitchQuat);

	front = Vector3::Transform(front, pitch);
	up = Vector3::Transform(up, pitch);

	const Matrix view = XMMatrixLookToLH(position, front, up);

	Renderer* const pRenderer = Renderer::GetInstance();

	const int width = pRenderer->GetWidth();
	const int height = pRenderer->GetHeight();

	Matrix proj;
	if (mbOrthogonoal)
	{
		proj = XMMatrixOrthographicLH(static_cast<float>(width), static_cast<float>(height), mNearPlane, mFarPlane);
	}
	else
	{
		const float aspectRatio = width / static_cast<float>(height);

		proj = XMMatrixPerspectiveFovLH(mFov, aspectRatio, mNearPlane, mFarPlane);
	}

	buffer.viewProj = view * proj;

	pRenderer->UpdateCBFrame(buffer);
}

void CameraComponent::DrawUI()
{

}
