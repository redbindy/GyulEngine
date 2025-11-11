#include "CameraComponent.h"

#include "Core/MathHelper.h"
#include "../Actor.h"
#include "Renderer/Renderer.h"
#include "UI/ImGuiHeaders.h"

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
