#include "MeshComponent.h"

#include "Renderer/Renderer.h"
#include "Core/InteractionSystem.h"

#include "../Actor.h"
#include "../Scene.h"
#include "UI/ImGuiHeaders.h"
#include "Resources/ModelManager.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/ShaderManager.h"

MeshComponent::MeshComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder)
	: Component(pOwner, label, updateOrder)
	, mpModel(ModelManager::GetInstance().GetModelOrNull("Cube"))
	, mbModelSelecting(false)
	, mbVSSelecting(false)
	, mbPSSelecting(false)
{
	Scene& scene = pOwner->GetScene();

	Renderer& renderer = Renderer::GetInstance();

	renderer.AddMeshComponent(scene.GetName(), this);

	InteractionSystem& interactionSystem = InteractionSystem::GetInstance();

	interactionSystem.RegisterCollider(
		scene.GetName(),
		pOwner,
		mpModel->GetBoundingRadiusLocal()
	);
}

MeshComponent::~MeshComponent()
{
	Actor& owner = GetOwner();
	Scene& scene = owner.GetScene();

	InteractionSystem& interactionSystem = InteractionSystem::GetInstance();

	interactionSystem.UnregisterCollider(
		scene.GetName(),
		&owner
	);

	Renderer& renderer = Renderer::GetInstance();

	renderer.RemoveMeshComponent(scene.GetName(), this);
}

void MeshComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);
}

void MeshComponent::SubmitRenderCommand() const
{
	const ModelData& modelData = mpModel->GetModelData();

	Actor& owner = GetOwner();

	const Matrix offset = Matrix::CreateTranslation(mpModel->GetBoundingRadiusLocal().Center * -1.0f);
	const Matrix worldMatrix = offset * owner.GetTransform();

	Renderer& renderer = Renderer::GetInstance();

	for (const std::pair<Mesh*, Material*>& pair : modelData)
	{
		Renderer::RenderCommand renderCommand;
		renderCommand.pMesh = pair.first;
		renderCommand.pMaterial = pair.second;
		renderCommand.worldMatrix = worldMatrix;

		renderer.EnqueueRenderCommand(renderCommand);
	}
}

void MeshComponent::DrawEditorUI()
{
	if (ImGui::TreeNodeEx(GetLabel(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Model: %s", mpModel->GetPath().c_str());

		if (ImGui::Button(UTF8_TEXT("¸ðµ¨ º¯°æ")))
		{
			mbModelSelecting = true;
		}

		ImGui::SameLine();

		if (ImGui::Button(UTF8_TEXT("Á¤Á¡ ¼ÎÀÌ´õ º¯°æ")))
		{
			mbVSSelecting = true;
		}

		ImGui::SameLine();

		if (ImGui::Button(UTF8_TEXT("ÇÈ¼¿ ¼ÎÀÌ´õ º¯°æ")))
		{
			mbPSSelecting = true;
		}

		if (mbModelSelecting)
		{
			Model* pOldModel = mpModel;

			ModelManager& modelManager = ModelManager::GetInstance();

			if (modelManager.DrawModelSelectorPopupAndSelectModel(mpModel))
			{
				mbModelSelecting = false;

				// update collider
				Actor& owner = GetOwner();
				Scene& scene = owner.GetScene();

				InteractionSystem& interactionSystem = InteractionSystem::GetInstance();

				interactionSystem.UpdateColliderRadius(
					scene.GetName(),
					&owner,
					mpModel->GetBoundingRadiusLocal()
				);
			}
		}

		if (mbVSSelecting || mbPSSelecting)
		{
			ShaderManager& shaderManager = ShaderManager::GetInstance();

			std::string newPath = "";
			if (shaderManager.DrawShaderSelectorPopupAndSelectShaders(/*out*/ newPath, mbPSSelecting))
			{
				if (!newPath.empty())
				{
					const ModelData& modelData = mpModel->GetModelData();
					for (const std::pair<Mesh*, Material*>& pair : modelData)
					{
						if (mbPSSelecting)
						{
							pair.second->SetPixelShaderPath(newPath);
						}
						else
						{
							pair.second->SetVertexShaderPath(newPath);
						}
					}
				}

				mbPSSelecting = false;
				mbVSSelecting = false;
			}
		}

		const ModelData& modelData = mpModel->GetModelData();

		for (const std::pair<Mesh*, Material*>& pair : modelData)
		{
			pair.first->DrawEditorUI();
			pair.second->DrawEditorUI();
		}

		ImGui::TreePop();
	}
}

void MeshComponent::CloneFrom(const Component& other)
{
	ASSERT(strcmp(GetLabel(), other.GetLabel()) == 0);

	if (this != &other)
	{
		Component::CloneFrom(other);

		const MeshComponent& otherMeshComp = static_cast<const MeshComponent&>(other);

		mpModel = otherMeshComp.mpModel;
	}
}

BoundingSphere MeshComponent::GetBoundingSphereWorld() const
{
	Actor& owner = GetOwner();

	const Matrix worldMatrix = owner.GetTransform();

	const BoundingSphere boundingSphereLocal = mpModel->GetBoundingRadiusLocal();

	BoundingSphere boundingSphereWorld;
	boundingSphereLocal.Transform(boundingSphereWorld, worldMatrix);

	return boundingSphereWorld;
}

