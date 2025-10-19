#include "MeshComponent.h"

#include "DebugHelper.h"
#include "GameCore.h"
#include "Renderer/Renderer.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"
#include "Actor.h"

MeshComponent::MeshComponent(Actor* const pOwner)
	: Component(pOwner, "MeshComponent")
	, mpMesh(nullptr)
	, mpMaterial(nullptr)
{
	Renderer& renderer = Renderer::GetInstance();

	// mpMesh = renderer->GetMeshOrNull(TEXT("Triangle"));
	mpMesh = renderer.GetMeshOrNull(TEXT("Cube"));
	ASSERT(mpMesh != nullptr);

	mpMaterial = renderer.GetMaterialOrNull(TEXT("Basic"));
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

	Actor* const pActor = GetOwner();

	const Matrix transform = pActor->GetTransform();

	Renderer& renderer = Renderer::GetInstance();
	renderer.UpdateCBWorldMatrix({ transform.Transpose() });

	deviceContext.DrawIndexed(mpMesh->GetIndexCount(), 0, 0);
}

void MeshComponent::DrawUI()
{
	Component::DrawUI();

	const char* const label = GetLabel();

	ImGui::PushID(label);
	if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		mpMesh->DrawUI();
		mpMaterial->DrawUI();

		ImGui::TreePop();
	}
	ImGui::PopID();
}