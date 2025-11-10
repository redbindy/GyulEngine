#pragma once

#include "Component.h"

class Mesh;
class Material;

class MeshComponent final : public Component
{
public:
	MeshComponent(Actor* const pOwner, const char* const label, const uint32_t updateOrder = 10u);
	virtual ~MeshComponent() = default;

	virtual void Update(const float deltaTime) override;

	void RequestRender() const;

private:
	Mesh* mpMesh;
	Material* mpMaterial;

private:
	MeshComponent(const MeshComponent& other) = delete;
	MeshComponent& operator=(const MeshComponent& other) = delete;
	MeshComponent(MeshComponent&& other) = delete;
	MeshComponent& operator=(MeshComponent&& other) = delete;
};