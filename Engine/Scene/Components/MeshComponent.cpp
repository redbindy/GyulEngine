#include "MeshComponent.h"

#include "Resources/MeshManager.h"
#include "Resources/MaterialManager.h"

#include "Renderer/Renderer.h"

#include "../Actor.h"
#include "../Scene.h"
#include "UI/ImGuiHeaders.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"

MeshComponent::MeshComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder)
	: Component(pOwner, label, updateOrder)
	, mpMesh(MeshManager::GetInstance().GetMeshOrNull("Cube"))
	, mpMaterial(MaterialManager::GetInstance().GetMaterialOrNull("Default"))
{
	Scene& scene = pOwner->GetScene();

	scene.AddMeshComponent(this);
}

MeshComponent::~MeshComponent()
{
	Actor& owner = GetOwner();

	Scene& scene = owner.GetScene();

	scene.RemoveMeshComponent(this);
}

void MeshComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);
}

void MeshComponent::RequestRender() const
{
	ASSERT(mpMesh != nullptr);
	ASSERT(mpMaterial != nullptr);

	Actor& owner = GetOwner();

	Renderer::RenderCommand renderCommand;
	renderCommand.pMesh = mpMesh;
	renderCommand.pMaterial = mpMaterial;
	renderCommand.worldMatrix = owner.GetTransform();

	Renderer& renderer = Renderer::GetInstance();

	renderer.EnqueueRenderCommand(renderCommand);
}

void MeshComponent::DrawEditorUI()
{
	if (ImGui::TreeNodeEx(GetLabel(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		mpMesh->DrawEditorUI();
		mpMaterial->DrawEditorUI();

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

		mpMesh = otherMeshComp.mpMesh;
		mpMaterial = otherMeshComp.mpMaterial;
	}
}

