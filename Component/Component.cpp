#include "Component.h"

#include "DebugHelper.h"
#include "Actor.h"

Component::Component(Actor* const pOwner, const char* const label)
	: mLabel(label)
	, mbAlive(true)
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

void Component::DrawUI()
{
	ImGui::Separator();

	ImGui::PushID(mLabel);
	{
		ImGui::Text(mLabel);
		constexpr const char* const REMOVE_LABEL = "RemoveComponent";

		const ImVec2 buttonSize = ImGui::CalcTextSize(REMOVE_LABEL);
		const ImVec2 region = ImGui::GetContentRegionAvail();
		const ImGuiStyle& style = ImGui::GetStyle();

		ImGui::SameLine(ImGui::GetCursorPosX() + region.x - (buttonSize.x + style.ItemSpacing.x));

		mbAlive = !ImGui::Button(REMOVE_LABEL);
	}
	ImGui::PopID();
}