#include "Component/CameraComponent.h"

#include "Renderer/Renderer.h"
#include "Actor.h"

CameraComponent::CameraComponent(Actor* const pOwner)
	: Component(pOwner, "CameraComponent")
	, mbActive(false)
	, mbOrthogonal(false)
	, mViewWidth(1.f)
	, mViewHeight(1.f)
	, mNearZ(0.1f)
	, mFarZ(100.f)
	, mFov(XMConvertToRadians(105.f))
{
	Renderer& renderer = Renderer::GetInstance();

	renderer.AddCameraComponent(this);
}

CameraComponent::~CameraComponent()
{
	Renderer& renderer = Renderer::GetInstance();

	renderer.RemoveCameraComponent(this);
}

void CameraComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	if (!mbActive)
	{
		return;
	}

	Actor& owner = *GetOwner();

	const Vector3 position = owner.GetPosition();

	const Vector3 front = Vector3::Transform(Vector3::UnitZ, owner.GetRotation());
	const Vector3 up = Vector3::Transform(Vector3::UnitY, owner.GetRotation());

	const Matrix view = XMMatrixLookToLH(position, front, up);

	Renderer& renderer = Renderer::GetInstance();

	const int width = renderer.GetWidth();
	const int height = renderer.GetHeight();

	Matrix proj;
	if (mbOrthogonal)
	{
		proj = XMMatrixOrthographicLH(mViewWidth, mViewHeight, mNearZ, mFarZ);
	}
	else
	{
		const float aspectRatio = width / static_cast<float>(height);

		proj = XMMatrixPerspectiveFovLH(mFov, aspectRatio, mNearZ, mFarZ);
	}

	const Matrix viewProj = view * proj;

	renderer.UpdateCBFrame(position, viewProj);

	mbActive = false;
}

void CameraComponent::DrawUI()
{
	Component::DrawUI();

	const char* const label = GetLabel();

	ImGui::PushID(label);
	if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::RadioButton("Perspective", !mbOrthogonal))
		{
			mbOrthogonal = false;
		}

		ImGui::SameLine();

		if (ImGui::RadioButton("Orthogonal", mbOrthogonal))
		{
			mbOrthogonal = true;
		}

		if (!mbOrthogonal)
		{
			mFov = XMConvertToDegrees(mFov);
			ImGui::DragFloat("Fov", &mFov, 0.5f, 85.f, 105.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			mFov = XMConvertToRadians(mFov);
		}
		else
		{
			Renderer& renderer = Renderer::GetInstance();

			const float screenWidth = static_cast<float>(renderer.GetWidth());
			const float screenHeight = static_cast<float>(renderer.GetHeight());

			ImGui::DragFloat("ViewWidth", &mViewWidth, 0.5f, 0.01f, screenWidth, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat("ViewHeight", &mViewHeight, 0.5f, 0.01f, screenHeight, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		}

		ImGui::DragFloat("NearZ", &mNearZ, 0.5f, 0.01f, mFarZ - 0.01f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::DragFloat("FarZ", &mFarZ, 0.5f, mNearZ + 0.01f, 300.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::TreePop();
	}
	ImGui::PopID();
}
