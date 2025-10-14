#include "Component/CameraControllerComponent.h"

#include "GameCore.h"
#include "Renderer/Renderer.h"
#include "Actor.h"

CameraControllerComponent::CameraControllerComponent(Actor* const pOwner)
	: Component(pOwner, "CameraControllerComponent")
	, mPrevMousePosition{ 0.f, 0.f }
{
	GameCore* const pGameCore = GameCore::GetInstance();

	mPrevMousePosition = pGameCore->GetMousePosition();
}

void CameraControllerComponent::Update(const float deltaTime)
{
	GameCore* const pGameCore = GameCore::GetInstance();
	Actor* pOwner = GetOwner();

	constexpr float SPEED = 5;

	Vector3 nextPos = pOwner->GetPosition();

	const Vector3 rotation = pOwner->GetRotation();
	const Matrix yawPitch = Matrix::CreateFromYawPitchRoll({ rotation.x, rotation.y, 0.f });

	const Vector3 front = Vector3::Transform({ 0.f, 0.f, 1.f }, yawPitch);
	const Vector3 up = Vector3::Transform({ 0.f, 1.f, 0.f }, yawPitch);
	Vector3 right = up.Cross(front);
	right.Normalize();

	if (pGameCore->IsKeyPressed('W'))
	{
		nextPos += front * SPEED * deltaTime;
	}

	if (pGameCore->IsKeyPressed('S'))
	{
		nextPos -= front * SPEED * deltaTime;
	}

	if (pGameCore->IsKeyPressed('A'))
	{
		nextPos -= right * SPEED * deltaTime;
	}

	if (pGameCore->IsKeyPressed('D'))
	{
		nextPos += right * SPEED * deltaTime;
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

}
