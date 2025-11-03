#include "Controller2DComponent.h"

#include "ComponentGenerator.h"
#include "GameCore.h"
#include "Actor.h"

Controller2DComponent::Controller2DComponent(Actor* const pOwner)
	: Component(pOwner, ComponentGenerator::GetNameString(ComponentGenerator::EComponentType::CONTROLLER_2D))
{
	ASSERT(pOwner != nullptr);
}

void Controller2DComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	constexpr float SPEED = 10.f;

	const float deltaMove = SPEED * deltaTime;

	GameCore& gameCore = GameCore::GetInstance();

	Vector2 dir(0.f, 0.f);
	if (gameCore.IsKeyPressed(VK_LEFT))
	{
		dir.x += -1;
	}

	if (gameCore.IsKeyPressed(VK_RIGHT))
	{
		dir.x += 1;
	}

	if (gameCore.IsKeyPressed(VK_UP))
	{
		dir.y += 1;
	}

	if (gameCore.IsKeyPressed(VK_DOWN))
	{
		dir.y += -1;
	}

	constexpr float ANGLE_SPEED = XM_PI;

	float deltaAngle = 0.f;
	if (gameCore.IsKeyPressed('C'))
	{
		deltaAngle -= ANGLE_SPEED;
	}

	if (gameCore.IsKeyPressed('Z'))
	{
		deltaAngle += ANGLE_SPEED;
	}

	dir.Normalize();

	Actor& actor = *GetOwner();

	const Vector3 currPosition = actor.GetPosition();
	const Vector2 moveVector = dir * deltaMove;

	actor.SetPosition(currPosition + Vector3(moveVector.x, moveVector.y, 0.f));

	const Quaternion quatRotation = Quaternion::CreateFromAxisAngle(Vector3::UnitZ, deltaAngle * deltaTime);
	const Quaternion rotationResult = Quaternion::Concatenate(quatRotation, actor.GetRotation());

	actor.SetRotation(rotationResult);
}


