#include "MeshComponent.h"

#include "DebugHelper.h"
#include "GameCore.h"
#include "Renderer/Renderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Actor.h"
#include "StringHelper.h"

MeshComponent::MeshComponent(Actor* const pOwner)
	: Component(pOwner, ComponentGenerator::GetNameString(ComponentGenerator::EComponentType::MESH))
	, mpMesh(nullptr)
	, mpMaterial(nullptr)
{
	Renderer& renderer = Renderer::GetInstance();

	// mpMesh = renderer->GetMeshOrNull("Triangle");
	mpMesh = renderer.GetMeshOrNull("Cube");
	// mpMesh = renderer.GetMeshOrNull("Sphere");
	ASSERT(mpMesh != nullptr);

	// mpMaterial = renderer.GetMaterialOrNull("Basic");
	mpMaterial = renderer.GetMaterialOrNull("ClassicLighting");
	ASSERT(mpMaterial != nullptr);

	renderer.AddMeshComponent(this);

	GameCore& gameCore = GameCore::GetInstance();
	gameCore.RegisterInteraction(pOwner, mpMesh->GetBoundingSphereLocal());
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