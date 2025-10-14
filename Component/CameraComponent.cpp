#include "Component/CameraComponent.h"

#include "Renderer/Renderer.h"
#include "Actor.h"

CameraComponent::CameraComponent(Actor* const pOwner)
	: Component(pOwner, "CameraComponent")
	, mbOrthogonal(false)
	, mNearZ(0.1f)
	, mFarZ(100.f)
	, mFov(XMConvertToRadians(105.f))
{
	Renderer* const pRenderer = Renderer::GetInstance();

	pRenderer->SetMainCamera(this);
}

void CameraComponent::Update(const float deltaTime)
{
	Actor* const pOwner = GetOwner();

	CBFrame buffer;

	const Vector3 position = pOwner->GetPosition();
	buffer.cameraPos = position;

	const Vector3 rotation = pOwner->GetRotation();

	const Matrix yawPitch = Matrix::CreateFromYawPitchRoll({ rotation.x, rotation.y, 0.f });

	Vector3 front(0.f, 0.f, 1.f);
	Vector3 up(0.f, 1.f, 0.f);

	front = Vector3::Transform(front, yawPitch);
	up = Vector3::Transform(up, yawPitch);

	const Matrix view = XMMatrixLookToLH(position, front, up);

	Renderer* const pRenderer = Renderer::GetInstance();

	const int width = pRenderer->GetWidth();
	const int height = pRenderer->GetHeight();

	Matrix proj;
	if (mbOrthogonal)
	{
		constexpr float NDC_WIDTH = 2.f;
		constexpr float NDC_HEIGHT = 2.f;

		proj = XMMatrixOrthographicLH(NDC_WIDTH, NDC_HEIGHT, mNearZ, mFarZ);
	}
	else
	{
		const float aspectRatio = width / static_cast<float>(height);

		proj = XMMatrixPerspectiveFovLH(mFov, aspectRatio, mNearZ, mFarZ);
	}

	const Matrix viewProj = view * proj;

	buffer.viewProj = viewProj.Transpose();

	pRenderer->UpdateCBFrame(buffer);
}

void CameraComponent::DrawUI()
{
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
			ImGui::DragFloat("ViewWidth", &mViewWidth, 0.5f, 0.01f, 2.f, "0.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::DragFloat("ViewHeight", &mViewHeight, 0.5f, 0.01f, 2.f, "0.2f", ImGuiSliderFlags_AlwaysClamp);
		}

		ImGui::DragFloat("NearZ", &mNearZ, 0.5f, 0.01f, mFarZ - 0.01f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::DragFloat("FarZ", &mFarZ, 0.5f, mNearZ + 0.01f, 300.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::TreePop();
	}
	ImGui::PopID();
}
