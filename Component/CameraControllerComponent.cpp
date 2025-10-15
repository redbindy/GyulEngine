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
	GameCore* const pGameCore = GameCore::GetInstance();

	mPrevMousePosition = pGameCore->GetMousePosition();
}

void CameraControllerComponent::Update(const float deltaTime)
{
	GameCore* const pGameCore = GameCore::GetInstance();
	Actor* pOwner = GetOwner();

	Vector3 nextPos = pOwner->GetPosition();

	const Vector3 rotation = pOwner->GetRotation();
	const Matrix yawPitch = Matrix::CreateFromYawPitchRoll({ rotation.x, rotation.y, 0.f });

	const Vector3 front = Vector3::Transform({ 0.f, 0.f, 1.f }, yawPitch);
	const Vector3 up = Vector3::Transform({ 0.f, 1.f, 0.f }, yawPitch);
	Vector3 right = up.Cross(front);
	right.Normalize();

	if (pGameCore->IsKeyPressed(mMoveKeys[static_cast<uint8_t>(EMoveKey::FRONT)]))
	{
		nextPos += front * mSpeed * deltaTime;
	}

	if (pGameCore->IsKeyPressed(mMoveKeys[static_cast<uint8_t>(EMoveKey::BACK)]))
	{
		nextPos -= front * mSpeed * deltaTime;
	}

	if (pGameCore->IsKeyPressed(mMoveKeys[static_cast<uint8_t>(EMoveKey::LEFT)]))
	{
		nextPos -= right * mSpeed * deltaTime;
	}

	if (pGameCore->IsKeyPressed(mMoveKeys[static_cast<uint8_t>(EMoveKey::RIGHT)]))
	{
		nextPos += right * mSpeed * deltaTime;
	}

	pOwner->SetPosition(nextPos);

	const Vector2 mousePosition = pGameCore->GetMousePosition();
	if (pGameCore->IsKeyPressed(VK_MBUTTON))
	{
		const Vector2 delta = mousePosition - mPrevMousePosition;

		Renderer* const pRenderer = Renderer::GetInstance();

		const Vector2 screenSize(static_cast<float>(pRenderer->GetWidth()), static_cast<float>(pRenderer->GetHeight()));

		const Vector2 deltaRadian = delta / screenSize * XM_2PI;

		// y = yaw, x = pitch
		const Vector3 deltaRotation(deltaRadian.y, deltaRadian.x, 0.f);

		pOwner->SetRotation(rotation + deltaRotation);
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
