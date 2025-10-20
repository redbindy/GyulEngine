#include "Component/CameraControllerComponent.h"

#include "GameCore.h"
#include "Renderer/Renderer.h"
#include "Actor.h"

CameraControllerComponent::CameraControllerComponent(Actor* const pOwner)
	: Component(pOwner, "CameraControllerComponent")
	, mPrevMousePosition{ 0.f, 0.f }
	, mSpeed(3.f)
	, mMoveKeys{ 'W', 'S', 'A', 'D' }
{
	GameCore& gameCore = GameCore::GetInstance();

	mPrevMousePosition = gameCore.GetMousePosition();
}

void CameraControllerComponent::Update(const float deltaTime)
{
	GameCore& gameCore = GameCore::GetInstance();
	Actor& owner = *GetOwner();

	Vector3 nextPos = owner.GetPosition();

	const Vector3 front = Vector3::Transform(Vector3::UnitZ, owner.GetRotation());
	const Vector3 up = Vector3::Transform(Vector3::UnitY, owner.GetRotation());
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

		const Vector2 deltaRadian = delta / screenSize * XM_2PI;

		// y = yaw, x = pitch
		const Quaternion yaw = Quaternion::CreateFromAxisAngle(up, deltaRadian.y);
		const Vector3 newRight = Vector3::Transform(right, yaw);

		const Quaternion pitch = Quaternion::CreateFromAxisAngle(newRight, deltaRadian.x);

		const Quaternion yawPitch = Quaternion::Concatenate(pitch, yaw);

		Quaternion resultRotation = Quaternion::Concatenate(yawPitch, owner.GetRotation());
		resultRotation.Normalize();

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
