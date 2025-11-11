#include "ComponentFactory.h"

#include "MeshComponent.h"
#include "CameraComponent.h"
#include "CameraControllerComponent.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"

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
	, mSelectedComponentIndex(0)
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

void ComponentFactory::DrawAddComponentUI(Actor* const pActor)
{
	ASSERT(pActor != nullptr);

	const char* const popupLabel = UTF8_TEXT("##컴포넌트");

	if (ImGui::BeginCombo(popupLabel, mComponentNames[mSelectedComponentIndex]))
	{
		for (int i = 0; i < static_cast<int>(mComponentNames.size()); ++i)
		{
			const bool isSelected = (mSelectedComponentIndex == i);
			if (ImGui::Selectable(mComponentNames[i], isSelected))
			{
				mSelectedComponentIndex = i;
			}

			// 선택된 항목을 기본으로 표시
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	// 컴포넌트 추가 버튼
	if (ImGui::Button(UTF8_TEXT("추가")))
	{
		const char* const selectedComponentName = mComponentNames[mSelectedComponentIndex];

		CreateComponentAlloc(selectedComponentName, pActor);
	}
}
