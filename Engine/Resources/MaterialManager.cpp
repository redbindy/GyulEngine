#include "MaterialManager.h"

#include "Material.h"
#include "Core/LogHelper.h"

enum
{
	DEFAULT_BUFFER_SIZE = 32
};

MaterialManager* MaterialManager::spInstance = nullptr;

MaterialManager::MaterialManager(ID3D11Device& device)
	: mDevice(device)
	, mMaterialMap()
{
	mMaterialMap.reserve(DEFAULT_BUFFER_SIZE);

	CreateMaterial(
		"Default",
		"./Assets/Default.dds",
		"./Shaders/VSBasic.hlsl",
		"./Shaders/PSBasic.hlsl"
	);
}

MaterialManager::~MaterialManager()
{
	for (std::pair<const std::string, Material*>& pair : mMaterialMap)
	{
		delete pair.second;
	}
}

Material* MaterialManager::CreateMaterial(
	const std::string& path,
	const std::string& texturePath,
	const std::string& vertexShaderPath,
	const std::string& pixelShaderPath
)
{
#define MAP_ITER std::unordered_map<std::string, Material*>::const_iterator

	MAP_ITER iter = mMaterialMap.find(path);

	if (iter != mMaterialMap.end())
	{
		return iter->second;
	}

#undef MAP_ITER

	// Material constant buffer »ý¼º
	ComPtr<ID3D11Buffer> materialBufferGPU;
	{
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(Material::CBMaterial);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		HRESULT hr = mDevice.CreateBuffer(&bufferDesc, nullptr, materialBufferGPU.GetAddressOf());

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreateBuffer - Material CB");
			ASSERT(false);
		}
	}

	Material* const pMaterial = new Material(
		path,
		texturePath,
		vertexShaderPath,
		pixelShaderPath,
		materialBufferGPU
	);

	mMaterialMap.insert(std::make_pair(path, pMaterial));

	return pMaterial;
}

Material* MaterialManager::GetMaterialOrNull(const std::string& path) const
{
#define MAP_ITER std::unordered_map<std::string, Material*>::const_iterator

	MAP_ITER iter = mMaterialMap.find(path);

	if (iter != mMaterialMap.end())
	{
		return iter->second;
	}

#undef MAP_ITER

	return nullptr;
}

void MaterialManager::UnloadMaterial(const std::string& path)
{
#define MAP_ITER std::unordered_map<std::string, Material*>::iterator

	MAP_ITER iter = mMaterialMap.find(path);

	if (iter != mMaterialMap.end())
	{
		delete iter->second;

		mMaterialMap.erase(iter);
	}

#undef MAP_ITER
}

void MaterialManager::Initialize(ID3D11Device& device)
{
	ASSERT(spInstance == nullptr);

	spInstance = new MaterialManager(device);
}