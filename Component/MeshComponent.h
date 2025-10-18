#pragma once

#include "Component.h"
#include "IPickingListener.h"

class Mesh;
class Material;

class MeshComponent final : public Component, public IPickingListener
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

	virtual bool CheckCollision(const Ray& ray, float& outDist) override;
	virtual void OnCollision() override;

private:
	Mesh* mpMesh;
	Material* mpMaterial;

private:
	inline BoundingSphere getBoundingSphereWorld() const;
};