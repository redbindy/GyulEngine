#include "MeshComponent.h"

#include "Resources/MeshManager.h"
#include "Resources/MaterialManager.h"

#include "Renderer/Renderer.h"

#include "../Actor.h"
#include "../Scene.h"

MeshComponent::MeshComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder)
	: Component(pOwner, label, updateOrder)
	, mpMesh(MeshManager::GetInstance().GetMeshOrNull("Cube"))
	, mpMaterial(MaterialManager::GetInstance().GetMaterialOrNull("Default"))
{
	Scene& scene = pOwner->GetScene();

	scene.AddMeshComponent(this);
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
