#include "Model.h"

#include "Mesh.h"
#include "Material.h"
#include "Renderer/Renderer.h"

Model::Model(
	const std::string& path,
	const ModelData& data,
	const Vector3 center,
	const float radius
)
	: mPath(path)
	, mModelData(data)
	, mBoundingSphereLocal(center, radius)
{
	ASSERT(radius > 0.f);
}
