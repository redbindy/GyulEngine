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
	, mRotation(Quaternion::Identity)
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
		if (!pComponent->IsAlive())
		{
			delete pComponent;

			continue;
		}

		pComponent->Update(deltaTime);
		mPendingComponents.push_back(pComponent);
	}

	mComponents.swap(mPendingComponents);
	mPendingComponents.clear();
}

Matrix Actor::GetTransform() const
{
	const Matrix translation = Matrix::CreateTranslation(mPosition);
	const Matrix scaleMat = Matrix::CreateScale(mScale);
	const Matrix rotationMat = Matrix::CreateFromQuaternion(mRotation);

	const Matrix transform = scaleMat * rotationMat * translation;

	return transform;
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
	ImGui::Separator();

	ImGui::PushID(mLabel);
	{
		if (!mbRenaming)
		{
			ImGui::Text(mLabel);

			if (ImGui::IsItemHovered() &&
				(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) || ImGui::IsKeyPressed(ImGuiKey_F2)))
			{
				GameCore& gameCore = GameCore::GetInstance();

				gameCore.SuspendUpdates();

				mbRenaming = true;
			}
		}
		else
		{
			ImGui::SetKeyboardFocusHere();
			if (ImGui::InputText("##rename", mTempBuffer, MAX_LABEL_LENGTH, ImGuiInputTextFlags_EnterReturnsTrue)
				|| ImGui::IsItemDeactivatedAfterEdit())
			{
				if (strlen(mTempBuffer) != 0)
				{
					strcpy(mLabel, mTempBuffer);
				}

				GameCore& gameCore = GameCore::GetInstance();

				gameCore.ResumeUpdates();

				mbRenaming = false;
			}
		}

		constexpr const char* const REMOVE_LABEL = "RemoveActor";

		const ImVec2 buttonSize = ImGui::CalcTextSize(REMOVE_LABEL);
		const ImVec2 region = ImGui::GetContentRegionAvail();
		const ImGuiStyle& style = ImGui::GetStyle();

		ImGui::SameLine(ImGui::GetCursorPosX() + region.x - (buttonSize.x + style.ItemSpacing.x));

		mbAlive = !ImGui::Button(REMOVE_LABEL);

		if (ImGui::TreeNodeEx(mLabel, ImGuiTreeNodeFlags_Framed))
		{
			constexpr const char* const POPUP_LABEL = "Select Component";
			if (ImGui::Button("AddComponent"))
			{
				ImGui::OpenPopup(POPUP_LABEL);
			}

			bool bOpen = true;
			if (ImGui::BeginPopupModal(POPUP_LABEL, &bOpen, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Choose a component to add: ");
				ImGui::Separator();

				int selectedNumber = -1;

				constexpr int componentCount = ComponentGenerator::GetComponentCount();
				for (int i = 0; i < componentCount; ++i)
				{
					if (ImGui::Selectable(ComponentGenerator::COMPONENT_NAMES[i], selectedNumber == i))
					{
						selectedNumber = i;

						ComponentGenerator::sConstructors[i](this);
					}
				}

				ImGui::EndPopup();
			}

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

				Renderer& renderer = Renderer::GetInstance();

				const int width = renderer.GetWidth();

				ImGui::DragFloat3("Position", reinterpret_cast<float*>(&mPosition), 0.1f, width * -0.5f, width * 0.5f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

				ImGui::DragFloat3("Scale", reinterpret_cast<float*>(&mScale), 0.1f, -100.f, 100.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

				constexpr float DEGREE_TO_RADIAN_COFF = XM_PI / 180.f;
				constexpr float RADIAN_TO_DEGREE_COFF = 180.f / XM_PI;

				const float MAX_DEGREE = 360.f;

				Vector3 rotation = mRotation.ToEuler() * RADIAN_TO_DEGREE_COFF;
				ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&rotation), 0.1f, -MAX_DEGREE, MAX_DEGREE, "%.1f", ImGuiSliderFlags_WrapAround);
				mRotation = Quaternion::CreateFromYawPitchRoll(rotation * DEGREE_TO_RADIAN_COFF);

				ImGui::TreePop();
			}

			for (int i = 0; i < mComponents.size(); ++i)
			{
				Component* const pComponent = mComponents[i];

				ImGui::PushID(i);
				{
					pComponent->DrawUI();
				}
				ImGui::PopID();
			}

			ImGui::Separator();

			ImGui::TreePop();
		}
	}
	ImGui::PopID();
}
