#include "Actor.h"

#include "DebugHelper.h"
#include "GameCore.h"
#include "Renderer/Renderer.h"
#include "Component/Component.h"

enum
{
	DEFAULT_BUFFER_SIZE = 8,
};

Actor::Actor(const char* const label)
	: mLabel{ '\0', }
	, mbAlive(true)
	, mPosition(0.f, 0.f, 0.f)
	, mScale(1.f, 1.f, 1.f)
	, mRotation(0.f, 0.f, 0.f)
	, mComponents()
{
	ASSERT(label != nullptr);

	mComponents.reserve(DEFAULT_BUFFER_SIZE);

	strncpy(mLabel, label, MAX_LABEL_LENGTH);
	mLabel[MAX_LABEL_LENGTH - 1] = '\0';
}

Actor::~Actor()
{
	for (Component* const pComponent : mComponents)
	{
		delete pComponent;
	}
	mComponents.clear();
}

void Actor::Update(const float deltaTime)
{
	ASSERT(deltaTime >= 0.f);

	for (Component* const pComponent : mComponents)
	{
		pComponent->Update(deltaTime);
	}
}

void Actor::AddComponent(Component* const pComponent)
{
	ASSERT(pComponent != nullptr);

	mComponents.push_back(pComponent);
}

void Actor::RemoveComponent(Component* const pComponent)
{
	ASSERT(pComponent != nullptr);

#define VECTOR_ITER std::vector<Component*>::iterator

	const VECTOR_ITER iter = std::find(mComponents.begin(), mComponents.end(), pComponent);

	if (iter != mComponents.end())
	{
		delete (*iter);

		mComponents.erase(iter);
	}

#undef VECTOR_ITER
}

void Actor::DrawUI()
{
	ImGui::PushID(mLabel);
	{
		ImGui::Text(mLabel);
		constexpr const char* const REMOVE_LABEL = "Remove";

		const ImVec2 buttonSize = ImGui::CalcTextSize(REMOVE_LABEL);
		const ImVec2 region = ImGui::GetContentRegionAvail();
		const ImGuiStyle& style = ImGui::GetStyle();

		ImGui::SameLine(ImGui::GetCursorPosX() + region.x - (buttonSize.x + style.ItemSpacing.x));

		mbAlive = !ImGui::Button(REMOVE_LABEL);

		if (ImGui::TreeNodeEx(mLabel, ImGuiTreeNodeFlags_Framed))
		{
			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::BeginTable("XYZ", 3, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV))
				{
					for (const char* const tag : { "X", "Y", "Z" })
					{
						ImGui::TableSetupColumn(tag);
					}
					ImGui::TableHeadersRow();
				}
				ImGui::EndTable();

				Renderer* const pRenderer = Renderer::GetInstance();

				const int width = pRenderer->GetWidth();

				ImGui::DragFloat3("Position", reinterpret_cast<float*>(&mPosition), 0.1f, width * -0.5f, width * 0.5f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

				ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&mScale), 0.1f, -100.f, 100.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

				constexpr float DEGREE_TO_RADIAN_COFF = XM_PI / 180.f;
				constexpr float RADIAN_TO_DEGREE_COFF = 180.f / XM_PI;

				mRotation *= RADIAN_TO_DEGREE_COFF;
				{
					const float MAX_DEGREE = 360.f;

					ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&mRotation), 0.1f, -MAX_DEGREE, MAX_DEGREE, "%.1f", ImGuiSliderFlags_WrapAround);
				}
				mRotation *= DEGREE_TO_RADIAN_COFF;

				ImGui::TreePop();
			}

			for (Component* const pComponent : mComponents)
			{
				pComponent->DrawUI();
			}

			ImGui::TreePop();
		}
	}
	ImGui::PopID();
}
