#include "MeshComponent.h"

#include "DebugHelper.h"
#include "GameCore.h"
#include "Renderer/Renderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Actor.h"
#include "StringHelper.h"

MeshComponent::MeshComponent(
	Actor* const pOwner,
	const char* label,
	Mesh* const pMesh,
	Material* const pMaterial
)
	: Component(pOwner, label)
	, mpMesh(pMesh)
	, mpMaterial(pMaterial)
{
	ASSERT(pOwner != nullptr);
	ASSERT(label != nullptr);
	ASSERT(pMesh != nullptr);
	ASSERT(pMaterial != nullptr);

	Renderer& renderer = Renderer::GetInstance();

	renderer.AddMeshComponent(this);

	GameCore& gameCore = GameCore::GetInstance();
	gameCore.RegisterInteraction(pOwner, mpMesh->GetBoundingSphereLocal());
}


MeshComponent::MeshComponent(Actor* const pOwner)
	: MeshComponent(
		pOwner,
		ComponentGenerator::GetNameString(ComponentGenerator::EComponentType::Mesh),
		// Renderer::GetInstance().GetMeshOrNull("Triangle"),
		Renderer::GetInstance().GetMeshOrNull("Cube"),
		// Renderer::GetInstance().GetMeshOrNull("Sphere"),
		// Renderer::GetInstance().GetMaterialOrNull("Basic")
		Renderer::GetInstance().GetMaterialOrNull("ClassicLighting")
	)
{
	ASSERT(pOwner != nullptr);
}

MeshComponent::~MeshComponent()
{
	Renderer& renderer = Renderer::GetInstance();
	renderer.RemoveMeshComponent(this);

	GameCore& gameCore = GameCore::GetInstance();
	gameCore.UnregisterInteraction(GetOwner());
}

void MeshComponent::Update(const float deltaTime)
{
	ASSERT(deltaTime > 0.f);
}

void MeshComponent::Draw(ID3D11DeviceContext& deviceContext) const
{
	mpMesh->Bind(deviceContext);
	mpMaterial->Bind(deviceContext);

	Actor& actor = *GetOwner();

	const Matrix transform = actor.GetTransform();

	Renderer& renderer = Renderer::GetInstance();
	renderer.UpdateCBWorldMatrix(transform);

	deviceContext.DrawIndexed(mpMesh->GetIndexCount(), 0, 0);
}

void MeshComponent::DrawUI()
{
	Component::DrawUI();

	const char* const label = GetLabel();

	ImGui::PushID(label);
	if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		Renderer& renderer = Renderer::GetInstance();

		mpMesh->DrawUI();
		mpMaterial->DrawUI();

		ImGui::TreePop();
	}
	ImGui::PopID();
}
