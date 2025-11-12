#include "Actor.h"

#include <cstring>
#include <algorithm>

#include "Core/Assert.h"
#include "./Components/Component.h"
#include "./Components/ComponentFactory.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"

Actor::Actor(Scene* const pScene, const char* const label)
	: mLabel{ '\0', }
	, mTempBuffer{ '\0', }
	, mbRenaming(false)
	, mbAlive(true)
	, mpScene(pScene)
	, mPosition(0.0f, 0.0f, 0.0f)
	, mScale(1.0f, 1.0f, 1.0f)
	, mRotation(Quaternion::Identity)
	, mpComponents()
	, mpPendingComponents()
{
	ASSERT(label != nullptr);
	ASSERT(strlen(label) < MAX_LABEL_LENGTH);

	strcpy(mLabel, label);
	mLabel[MAX_LABEL_LENGTH - 1] = '\0';
}

Actor::~Actor()
{
	// Delete all components
	for (Component* pComponent : mpComponents)
	{
		delete pComponent;
	}

	for (Component* pComponent : mpPendingComponents)
	{
		delete pComponent;
	}
}

Actor& Actor::operator=(const Actor& other)
{
	if (this != &other)
	{
		strcpy(mLabel, other.mLabel);
		mLabel[MAX_LABEL_LENGTH - 1] = '\0';

		mbRenaming = other.mbRenaming;
		mbAlive = other.mbAlive;
		mPosition = other.mPosition;
		mScale = other.mScale;
		mRotation = other.mRotation;

		for (Component* pComponent : mpComponents)
		{
			delete pComponent;
		}
		mpComponents.clear();

		for (Component* pComponent : other.mpComponents)
		{
			Component* pNewComponent = ComponentFactory::GetInstance().CreateComponentAlloc(pComponent->GetLabel(), this);

			pNewComponent->CloneFrom(*pComponent);
		}
	}

	return *this;
}

void Actor::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);

	if (!mbAlive)
	{
		return;
	}

	for (Component* pComponent : mpComponents)
	{
		pComponent->Update(deltaTime);
	}
}

Matrix Actor::GetTransform() const
{
	const Matrix scale = Matrix::CreateScale(mScale);
	const Matrix rotation = Matrix::CreateFromQuaternion(mRotation);
	const Matrix translation = Matrix::CreateTranslation(mPosition);

	return scale * rotation * translation;
}

void Actor::AddComponent(Component* const pComponent)
{
	ASSERT(pComponent != nullptr);

#define VECTOR_ITER std::vector<Component*>::iterator

	VECTOR_ITER iter = mpComponents.begin();
	while (iter != mpComponents.end())
	{
		if (pComponent->GetUpdateOrder() < (*iter)->GetUpdateOrder())
		{
			break;
		}

		++iter;
	}

	mpComponents.insert(iter, pComponent);

#undef VECTOR_ITER
}

void Actor::RemoveComponent(Component* const pComponent)
{
	ASSERT(pComponent != nullptr);

#define VECTOR_ITER std::vector<Component*>::iterator

	VECTOR_ITER iter = std::find(mpComponents.begin(), mpComponents.end(), pComponent);

	if (iter != mpComponents.end())
	{
		delete* iter;

		mpComponents.erase(iter);

		return;
	}

	iter = std::find(mpPendingComponents.begin(), mpPendingComponents.end(), pComponent);
	if (iter != mpPendingComponents.end())
	{
		delete* iter;

		mpPendingComponents.erase(iter);
	}

#undef VECTOR_ITER
}

void Actor::DrawEditorUI()
{
	ImGui::PushID(mLabel);

	// Actor as a tree node so its components can be collapsed/expanded
	if (ImGui::TreeNodeEx(mLabel, ImGuiTreeNodeFlags_DefaultOpen))
	{
		ComponentFactory& componentFactory = ComponentFactory::GetInstance();
		componentFactory.DrawAddComponentUI(this);

		if (ImGui::TreeNodeEx(UTF8_TEXT("변환"), ImGuiTreeNodeFlags_DefaultOpen))
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

			ImGui::DragFloat3(UTF8_TEXT("위치"), reinterpret_cast<float*>(&mPosition), 0.1f, -3000.f, 3000.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

			ImGui::DragFloat3(UTF8_TEXT("확대/축소"), reinterpret_cast<float*>(&mScale), 0.1f, -100.f, 100.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

			constexpr float DEGREE_TO_RADIAN_COFF = XM_PI / 180.f;
			constexpr float RADIAN_TO_DEGREE_COFF = 180.f / XM_PI;

			const float MAX_DEGREE = 360.f;

			Vector3 rotation = mRotation.ToEuler() * RADIAN_TO_DEGREE_COFF;
			ImGui::DragFloat3(UTF8_TEXT("회전"), reinterpret_cast<float*>(&rotation), 0.1f, -MAX_DEGREE, MAX_DEGREE, "%.1f", ImGuiSliderFlags_WrapAround);
			mRotation = Quaternion::CreateFromYawPitchRoll(rotation * DEGREE_TO_RADIAN_COFF);

			ImGui::TreePop();
		}

		for (int i = 0; i < mpComponents.size(); ++i)
		{
			ImGui::PushID(i);

			Component* const pComponent = mpComponents[i];
			if (ImGui::Button(UTF8_TEXT("삭제")))
			{
				delete pComponent;
			}
			else
			{
				mpPendingComponents.push_back(mpComponents[i]);

				pComponent->DrawEditorUI();
			}
			ImGui::PopID();
		}

		mpComponents.swap(mpPendingComponents);
		mpPendingComponents.clear();

		ImGui::TreePop();
	}

	ImGui::PopID();
}
