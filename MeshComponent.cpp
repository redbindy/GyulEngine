#include "MeshComponent.h"

#include "DebugHelper.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Material.h"
#include "Actor.h"

MeshComponent::MeshComponent(Actor* const pOwner)
	: Component(pOwner, "MeshComponent")
	, mpMesh(nullptr)
	, mpMaterial(nullptr)
{
	Renderer* const pRenderer = Renderer::GetInstance();

	mpMesh = pRenderer->GetMeshOrNull(TEXT("Triangle"));
	ASSERT(mpMesh != nullptr);

	mpMaterial = pRenderer->GetMaterialOrNull(TEXT("Basic"));
	ASSERT(mpMaterial != nullptr);

	pRenderer->AddMeshComponent(this);
}

MeshComponent::~MeshComponent()
{
	Renderer* const pRenderer = Renderer::GetInstance();

	pRenderer->RemoveMeshComponent(this);
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

	const Vector3 pos = pActor->GetPosition();
	const Matrix translation = Matrix::CreateTranslation(pos);

	const Vector3 scale = pActor->GetScale();
	const Matrix scaleMat = Matrix::CreateScale(scale);

	const Vector3 rotation = pActor->GetRotation();
	const Matrix rotationX = Matrix::CreateRotationX(rotation.x);
	const Matrix rotationY = Matrix::CreateRotationY(rotation.y);
	const Matrix rotationZ = Matrix::CreateRotationZ(rotation.z);

	const Matrix transform = scaleMat * rotationX * rotationY * rotationZ * translation;

	Renderer* pRenderer = Renderer::GetInstance();
	pRenderer->UpdateCBWorldMatrix({ transform.Transpose() });

	deviceContext.DrawIndexed(mpMesh->GetIndexCount(), 0, 0);
}

void MeshComponent::DrawUI()
{
	const char* label = GetLabel();

	ImGui::PushID(label);
	if (ImGui::BeginChild(label, { 0, 0 }, true))
	{
		ImGui::Text(label);

		mpMesh->DrawUI();
		mpMaterial->DrawUI();
	}
	ImGui::EndChild();
	ImGui::PopID();
}
