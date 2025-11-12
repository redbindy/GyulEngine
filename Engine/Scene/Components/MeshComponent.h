#pragma once

#include "Component.h"
#include "Core/MathHelper.h"

class Mesh;
class Material;
class Renderer;

class MeshComponent final : public Component
{
public:
	MeshComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder = 10u);
	virtual ~MeshComponent();

	virtual void Update(const float deltaTime) override;

	void SubmitRenderCommand() const;

	virtual void DrawEditorUI() override;

	virtual void CloneFrom(const Component& other) override;

	BoundingSphere GetBoundingSphereWorld() const;

private:
	Mesh* mpMesh;
	Material* mpMaterial;

private:
	MeshComponent(const MeshComponent& other) = delete;
	MeshComponent& operator=(const MeshComponent& other) = delete;
	MeshComponent(MeshComponent&& other) = delete;
	MeshComponent& operator=(MeshComponent&& other) = delete;
};