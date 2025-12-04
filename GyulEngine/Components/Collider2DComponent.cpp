#include "Collider2DComponent.h"

#include "../Scene/Actor.h"
#include "ScriptComponent.h"

UINT Collider2DComponent::sCollisionID = 0;

Collider2DComponent::Collider2DComponent(const std::wstring& name)
	: Component(name)
	, mID(sCollisionID++)
	, mOffset(0.f, 0.f)
	, mSize(Vector2::One)
{
}

void Collider2DComponent::OnCollisionEnter(Collider2DComponent* const pOther)
{
	Actor* const pOwner = GetOwner();

	ScriptComponent* const pScriptComp = pOwner->GetComponent<ScriptComponent>();

	pScriptComp->OnCollisionEnter(pOther);
}

void Collider2DComponent::OnCollisionStay(Collider2DComponent* const pOther)
{
	Actor* const pOwner = GetOwner();

	ScriptComponent* const pScriptComp = pOwner->GetComponent<ScriptComponent>();

	pScriptComp->OnCollisionStay(pOther);
}

void Collider2DComponent::OnCollisionExit(Collider2DComponent* const pOther)
{
	Actor* const pOwner = GetOwner();

	ScriptComponent* const pScriptComp = pOwner->GetComponent<ScriptComponent>();

	pScriptComp->OnCollisionExit(pOther);
}
