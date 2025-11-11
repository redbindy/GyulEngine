#include "Component.h"

#include "../Actor.h"

Component::Component(Actor* const pOwner)
	: Component(pOwner, "Component", 10)
{

}

Component::Component(Actor* const pOwner, const char* const label, const uint32_t updateOrder)
	: mLabel(label)
	, mbAlive(true)
	, mUpdateOrder(updateOrder)
	, mpOwner(pOwner)
{
	ASSERT(pOwner != nullptr);
	ASSERT(label != nullptr);

	pOwner->AddComponent(this);
}

void Component::CloneFrom(const Component& other)
{
	if (this != &other)
	{
		mLabel = other.mLabel;
		mbAlive = other.mbAlive;
		mUpdateOrder = other.mUpdateOrder;
	}
}
