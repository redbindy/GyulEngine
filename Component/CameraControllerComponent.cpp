#include "Component/CameraControllerComponent.h"

#include "GameCore.h"
#include "Renderer/Renderer.h"
#include "Actor.h"

CameraControllerComponent::CameraControllerComponent(Actor* const pOwner)
	: Component(pOwner, ComponentGenerator::GetNameString(ComponentGenerator::EComponentType::CameraController))
	, mPrevMousePosition{ 0.f, 0.f }
	, mSpeed(3.f)
	, mMoveKeys{ 'W', 'S', 'A', 'D' }
{
	ASSERT(pOwner != nullptr);

	GameCore& gameCore = GameCore::GetInstance();

	mPrevMousePosition = gameCore.GetMousePosition();
}

void CameraControllerComponent::Update(const float deltaTime)
{
	GameCore& gameCore = GameCore::GetInstance();
	Actor& owner = *GetOwner();

	Vector3 nextPos = owner.GetPosition();

	const Quaternion ownerRotation = owner.GetRotation();
	const Vector3 eulerRotation = ownerRotation.ToEuler();

	const Quaternion yawPitch = Quaternion::CreateFromYawPitchRoll(eulerRotation.y, eulerRotation.x, 0.f);

	const Vector3 front = Vector3::Transform(Vector3::UnitZ, yawPitch);
	const Vector3 up = Vector3::Transform(Vector3::UnitY, yawPitch);

	Vector3 right = up.Cross(front);
	right.Normalize();

	if (gameCore.IsKeyPressed(mMoveKeys[static_cast<uint8_t>(EMoveKey::FRONT)]))
	{
		nextPos += front * mSpeed * deltaTime;
	}

	if (gameCore.IsKeyPressed(mMoveKeys[static_cast<uint8_t>(EMoveKey::BACK)]))
	{
		nextPos -= front * mSpeed * deltaTime;
	}

	if (gameCore.IsKeyPressed(mMoveKeys[static_cast<uint8_t>(EMoveKey::LEFT)]))
	{
		nextPos -= right * mSpeed * deltaTime;
	}

	if (gameCore.IsKeyPressed(mMoveKeys[static_cast<uint8_t>(EMoveKey::RIGHT)]))
	{
		nextPos += right * mSpeed * deltaTime;
	}

	owner.SetPosition(nextPos);

	const Vector2 mousePosition = gameCore.GetMousePosition();
	if (gameCore.IsKeyPressed(VK_MBUTTON))
	{
		const Vector2 delta = mousePosition - mPrevMousePosition;

		Renderer& renderer = Renderer::GetInstance();

		const Vector2 screenSize(static_cast<float>(renderer.GetWidth()), static_cast<float>(renderer.GetHeight()));

		// dx - yaw, dy - pitch
		const Vector2 deltaRadian = delta / screenSize * XM_2PI;

		Vector3 resultEuler = eulerRotation + Vector3(deltaRadian.y, deltaRadian.x, 0.f);

		constexpr float MAX_YAW = XMConvertToRadians(89.f);
		constexpr float MAX_PITCH = XMConvertToRadians(89.f);
		constexpr Vector3 MAX_VECTOR = Vector3(MAX_PITCH, MAX_YAW, 0.f);

		resultEuler = XMVectorClamp(resultEuler, -MAX_VECTOR, MAX_VECTOR);

		const Quaternion resultRotation = Quaternion::CreateFromYawPitchRoll(resultEuler);

		owner.SetRotation(resultRotation);
	}
	mPrevMousePosition = mousePosition;
}

void CameraControllerComponent::DrawUI()
{
	Component::DrawUI();

	const char* const label = GetLabel();

	ImGui::PushID(label);
	if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("FollowCursor(Click Wheel)");

		ImGui::DragFloat("Speed", &mSpeed, 0.01f, 0.f, 10.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

		if (ImGui::BeginTable("Key", static_cast<uint8_t>(EMoveKey::COUNT), ImGuiTableFlags_Borders))
		{
			constexpr const char* const KEYS[static_cast<uint8_t>(EMoveKey::COUNT)] = {
				"Front", "Back", "Left", "Right"
			};

			for (const char* const header : KEYS)
			{
				ImGui::TableSetupColumn(header);
			}
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();

			for (int i = 0; i < static_cast<uint8_t>(EMoveKey::COUNT); ++i)
			{
				ImGui::TableSetColumnIndex(i);
				ImGui::SetNextItemWidth(-FLT_MIN);

				ImGui::PushID(KEYS[i]);
				{
					ImGui::Text("%c", mMoveKeys[i]);
				}
				ImGui::PopID();
			}
		}
		ImGui::EndTable();

		ImGui::TreePop();
	}
	ImGui::PopID();
}
