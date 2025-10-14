#include "Component.h"

#include "DebugHelper.h"
#include "Actor.h"

Component::Component(Actor* const pOwner, const char* const label)
	: mLabel(label)
	, mpOwner(pOwner)
{
	ASSERT(pOwner != nullptr);
	ASSERT(label != nullptr);

	pOwner->AddComponent(this);
}

Component::Component(Actor* const pOwner)
	: Component(pOwner, "Component")
{

}
