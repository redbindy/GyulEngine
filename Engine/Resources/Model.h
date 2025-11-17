#pragma once

#include <vector>
#include <string>

#include "Core/MathHelper.h"

class Mesh;
class Material;

using ModelData = std::vector<std::pair<Mesh*, Material*>>;

class Model final
{
public:
	Model(
		const std::string& path,
		const ModelData& data,
		const Vector3 mCenter,
		const float radius
	);
	~Model() = default;

	const ModelData& GetModelData() const
	{
		return mModelData;
	}

	const std::string& GetPath() const
	{
		return mPath;
	}

	BoundingSphere GetBoundingRadiusLocal() const
	{
		return mBoundingSphereLocal;
	}

private:
	std::string mPath;
	ModelData mModelData;
	BoundingSphere mBoundingSphereLocal;

private:
	Model(const Model& other) = delete;
	Model& operator=(const Model& other) = delete;
	Model(Model&& other) = delete;
	Model& operator=(Model&& other) = delete;
};