#include "CameraComponent.h"

#include "../Actor.h"
#include "Renderer/Renderer.h"
#include "UI/ImGuiHeaders.h"
#include "../Scene.h"

CameraComponent::CameraComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder)
	: Component(pOwner, label, updateOrder)
	, mbOrhographic(false)
	, mViewWidth(100.f)
	, mViewHeight(100.f)
	, mNearZ(0.1f)
	, mFarZ(100.f)
	, mFov(XMConvertToRadians(90.f))
{
}

void CameraComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	Renderer& renderer = Renderer::GetInstance();

	renderer.SetMainCameraComponent(this);
}

void CameraComponent::UpdateCameraInfomation()
{
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

	mViewProj = view * proj;

	// https://copynull.tistory.com/265

	// 가까운 평면
	float x = mViewProj._14 + mViewProj._13;
	float y = mViewProj._24 + mViewProj._23;
	float z = mViewProj._34 + mViewProj._33;
	float w = mViewProj._44 + mViewProj._43;

	Plane nearPlane(x, y, z, w);
	nearPlane.Normalize();

	mFrustumPlanes[0] = nearPlane; // Near

	// 먼 평면
	x = mViewProj._14 - mViewProj._13;
	y = mViewProj._24 - mViewProj._23;
	z = mViewProj._34 - mViewProj._33;
	w = mViewProj._44 - mViewProj._43;

	Plane farPlane(x, y, z, w);
	farPlane.Normalize();

	mFrustumPlanes[1] = farPlane; // Far

	// 왼쪽 평면
	x = mViewProj._14 + mViewProj._11;
	y = mViewProj._24 + mViewProj._21;
	z = mViewProj._34 + mViewProj._31;
	w = mViewProj._44 + mViewProj._41;

	Plane leftPlane(x, y, z, w);
	leftPlane.Normalize();

	mFrustumPlanes[2] = leftPlane; // Left

	// 오른쪽 평면
	x = mViewProj._14 - mViewProj._11;
	y = mViewProj._24 - mViewProj._21;
	z = mViewProj._34 - mViewProj._31;
	w = mViewProj._44 - mViewProj._41;

	Plane rightPlane(x, y, z, w);
	rightPlane.Normalize();

	mFrustumPlanes[3] = rightPlane; // Right

	// 윗 평면
	x = mViewProj._14 - mViewProj._12;
	y = mViewProj._24 - mViewProj._22;
	z = mViewProj._34 - mViewProj._32;
	w = mViewProj._44 - mViewProj._42;

	Plane topPlane(x, y, z, w);
	topPlane.Normalize();

	mFrustumPlanes[4] = topPlane; // Top

	// 아랫 평면
	x = mViewProj._14 + mViewProj._12;
	y = mViewProj._24 + mViewProj._22;
	z = mViewProj._34 + mViewProj._32;
	w = mViewProj._44 + mViewProj._42;

	Plane bottomPlane(x, y, z, w);
	bottomPlane.Normalize();

	mFrustumPlanes[5] = bottomPlane; // Bottom

	renderer.UpdateCBFrame(position, mViewProj);
}

void CameraComponent::DrawEditorUI()
{
	if (ImGui::TreeNodeEx(GetLabel(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::RadioButton(UTF8_TEXT("원근 투영"), !mbOrhographic))
		{
			mbOrhographic = false;
		}

		ImGui::SameLine();

		if (ImGui::RadioButton(UTF8_TEXT("정투영"), mbOrhographic))
		{
			mbOrhographic = true;
		}

		if (!mbOrhographic)
		{
			mFov = XMConvertToDegrees(mFov);
			ImGui::DragFloat(UTF8_TEXT("시야각"), &mFov, 0.5f, 1.f, 120.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			mFov = XMConvertToRadians(mFov);
		}
		else
		{
			Renderer& renderer = Renderer::GetInstance();

			const D3D11_VIEWPORT& viewport = renderer.GetViewport();

			ImGui::DragFloat(UTF8_TEXT("시야 너비"), &mViewWidth, 0.5f, 0.01f, viewport.Width, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat(UTF8_TEXT("시야 높이"), &mViewHeight, 0.5f, 0.01f, viewport.Height, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		}

		ImGui::DragFloat(UTF8_TEXT("NearZ"), &mNearZ, 0.5f, 0.01f, mFarZ - 0.01f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::DragFloat(UTF8_TEXT("FarZ"), &mFarZ, 0.5f, mNearZ + 0.01f, 300.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::TreePop();
	}
}

void CameraComponent::CloneFrom(const Component& other)
{
	ASSERT(strcmp(GetLabel(), other.GetLabel()) == 0);

	if (this != &other)
	{
		Component::CloneFrom(other);

		const CameraComponent& otherCamera = static_cast<const CameraComponent&>(other);

		mbOrhographic = otherCamera.mbOrhographic;
		mViewWidth = otherCamera.mViewWidth;
		mViewHeight = otherCamera.mViewHeight;
		mNearZ = otherCamera.mNearZ;
		mFarZ = otherCamera.mFarZ;
		mFov = otherCamera.mFov;
	}
}

bool CameraComponent::IsInViewFrustum(const BoundingSphere& sphereWorld) const
{
	// https://copynull.tistory.com/265

	for (int i = 0; i < ARRAYSIZE(mFrustumPlanes); ++i)
	{
		const Plane& plane = mFrustumPlanes[i];

		if (plane.DotCoordinate(sphereWorld.Center) < -sphereWorld.Radius)
		{
			return false;
		}
	}

	return true;
}
