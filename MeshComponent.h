#pragma once

#include "Component.h"

class Mesh;
class Material;

class MeshComponent final : public Component
{
public:
	MeshComponent(Actor* const pOwner);
	virtual ~MeshComponent();
	MeshComponent(const MeshComponent& other) = delete;
	MeshComponent& operator=(const MeshComponent& other) = delete;
	MeshComponent(MeshComponent&& other) = delete;
	MeshComponent& operator=(MeshComponent&& other) = delete;

	virtual void Update(const float deltaTime) override;
	void Draw(ID3D11DeviceContext& deviceContext) const;
	virtual void DrawUI() override;

private:
	Mesh* mpMesh;
	Material* mpMaterial;
};