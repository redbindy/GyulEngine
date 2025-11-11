#include "CameraControllerComponent.h"

#include "Core/InputSystem.h"
#include "../Actor.h"
#include "Renderer/Renderer.h"
#include "UI/ImGuiHeaders.h"

CameraControllerComponent::CameraControllerComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder)
	: Component(pOwner, label, updateOrder)
	, mMoveSpeed(5.f)
{

}

void CameraControllerComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	InputSystem& inputSystem = InputSystem::GetInstance();
	Actor& owner = GetOwner();

	const Quaternion ownerRotation = owner.GetRotation();
	const Vector3 eulerRotation = ownerRotation.ToEuler();

	const Quaternion yawQuat = Quaternion::CreateFromAxisAngle(Vector3::UnitY, eulerRotation.y);

	Vector3 front = Vector3::Transform(Vector3::UnitZ, yawQuat);

	Vector3 right = Vector3::UnitY.Cross(front);
	right.Normalize();

	const Quaternion pitchQuat = Quaternion::CreateFromAxisAngle(right, eulerRotation.x);

	front = Vector3::Transform(front, pitchQuat);
	const Vector3 up = Vector3::Transform(Vector3::UnitY, pitchQuat);

	Vector3 nextPos = owner.GetPosition();

	if (inputSystem.IsKeyPressed('W'))
	{
		nextPos += front * mMoveSpeed * deltaTime;
	}

	if (inputSystem.IsKeyPressed('S'))
	{
		nextPos -= front * mMoveSpeed * deltaTime;
	}

	if (inputSystem.IsKeyPressed('A'))
	{
		nextPos -= right * mMoveSpeed * deltaTime;
	}

	if (inputSystem.IsKeyPressed('D'))
	{
		nextPos += right * mMoveSpeed * deltaTime;
	}

	owner.SetPosition(nextPos);

	if (inputSystem.IsKeyPressed(VK_MBUTTON))
	{
		const Vector2 delta = inputSystem.GetMouseDelta();

		Renderer& renderer = Renderer::GetInstance();

		const D3D11_VIEWPORT& viewport = renderer.GetViewport();

		const Vector2 viewportSize = Vector2(
			viewport.Width,
			viewport.Height
		);

		// dx - yaw, dy - pitch
		const Vector2 deltaRadian = delta / viewportSize * XM_2PI;

		Vector3 resultEuler = eulerRotation + Vector3(deltaRadian.y, deltaRadian.x, 0.f);

		constexpr float MAX_YAW = XMConvertToRadians(359.f);
		constexpr float MAX_PITCH = XMConvertToRadians(89.f);
		constexpr Vector3 MAX_VECTOR = Vector3(MAX_PITCH, MAX_YAW, 0.f);

		resultEuler = XMVectorClamp(resultEuler, -MAX_VECTOR, MAX_VECTOR);

		const Quaternion resultRotation = Quaternion::CreateFromYawPitchRoll(resultEuler);

		owner.SetRotation(resultRotation);
	}
}

void CameraControllerComponent::DrawEditorUI()
{
	if (ImGui::TreeNodeEx(GetLabel(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat(UTF8_TEXT("이동 속도"), &mMoveSpeed, 0.1f, 0.1f, 100.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::TreePop();
	}
}

void CameraControllerComponent::CloneFrom(const Component& other)
{
	ASSERT(strcmp(GetLabel(), other.GetLabel()) == 0);

	if (this != &other)
	{
		Component::CloneFrom(other);

		const CameraControllerComponent& otherCameraController = static_cast<const CameraControllerComponent&>(other);

		mMoveSpeed = otherCameraController.mMoveSpeed;
	}
}
