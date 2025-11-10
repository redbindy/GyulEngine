#include "ComponentFactory.h"

#include "MeshComponent.h"
#include "CameraComponent.h"
#include "CameraControllerComponent.h"

#define COMPONENT_LIST \
	COMPONENT_ENTRY(MeshComponent) \
	COMPONENT_ENTRY(CameraComponent) \
	COMPONENT_ENTRY(CameraControllerComponent) \

ComponentFactory* ComponentFactory::spInstance = nullptr;

ComponentFactory::ComponentFactory()
	: mComponentNames
	{
	#define COMPONENT_ENTRY(type) #type,
		COMPONENT_LIST
	#undef COMPONENT_ENTRY
	}
	, mComponentConstructors
	{
#define COMPONENT_ENTRY(type) { #type, [](Actor* const pOwner) { return new type(pOwner, #type); } }, 
		COMPONENT_LIST
	#undef COMPONENT_ENTRY
	}
{

}

void ComponentFactory::Initialize()
{
	ASSERT(spInstance == nullptr);

	spInstance = new ComponentFactory();
}

Component* ComponentFactory::CreateComponentAlloc(const std::string& typeName, Actor* const pOwner)
{
	ASSERT(mComponentConstructors.find(typeName) != mComponentConstructors.end());

	return mComponentConstructors[typeName](pOwner);
}