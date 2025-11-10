#pragma once

#include <unordered_map>
#include <string>

#include <d3d11.h>

#include "Core/Assert.h"

class Material;

class MaterialManager final
{
public:
	Material* CreateMaterial(
		const std::string& path,
		const std::string& texturePath,
		const std::string& vertexShaderPath,
		const std::string& pixelShaderPath
	);
	Material* GetMaterialOrNull(const std::string& path) const;
	void UnloadMaterial(const std::string& path);

	// static
	static void Initialize(ID3D11Device& device);

	static MaterialManager& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static MaterialManager* spInstance;

	ID3D11Device& mDevice;

	std::unordered_map<std::string, Material*> mMaterialMap;

private:
	MaterialManager(ID3D11Device& device);
	~MaterialManager();

private:
	MaterialManager(const MaterialManager& other) = delete;
	MaterialManager& operator=(const MaterialManager& other) = delete;
	MaterialManager(MaterialManager&& other) = delete;
	MaterialManager& operator=(MaterialManager&& other) = delete;
};