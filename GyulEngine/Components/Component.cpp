#include "Component.h"

#include "../Core/Assert.h"

Component::Component(const std::wstring& name)
	: Entity(name)
	, mpOwner(nullptr)
{

}
