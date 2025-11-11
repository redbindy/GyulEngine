#include "TextureManager.h"

#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

#include "Texture.h"
#include "Core/StringHelper.h"
#include "Core/LogHelper.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"

enum
{
	DEFAULT_BUFFER_SIZE = 32
};

TextureManager* TextureManager::spInstance = nullptr;

TextureManager::TextureManager(ID3D11Device& device)
	: mDevice(device)
	, mTextureMap()
{
	mTextureMap.reserve(DEFAULT_BUFFER_SIZE);

	LoadTexture("./Assets/Default.dds");
}

TextureManager::~TextureManager()
{
	for (std::pair<const std::string, Texture*>& pair : mTextureMap)
	{
		delete pair.second;
	}
}

void TextureManager::LoadTexture(const std::string& path)
{
	using namespace DirectX;

#define MAP_ITER std::unordered_map<std::string, Texture*>::iterator

	MAP_ITER iter = mTextureMap.find(path);

	if (iter == mTextureMap.end())
	{
		char extension[MAX_PATH];
		GetExtension(path, extension);

		for (char& c : extension)
		{
			c = static_cast<char>(tolower(c));
		}

		TCHAR widePath[MAX_PATH];
		ConvertMultiToWide(widePath, path.c_str());

		ComPtr<ID3D11ShaderResourceView> shaderResourceViewGPU;
		ID3D11Resource* pResource;

		HRESULT hr;
		if (strcmp(extension, "dds") == 0)
		{
			hr = CreateDDSTextureFromFile(&mDevice, widePath, &pResource, shaderResourceViewGPU.GetAddressOf());
		}
		else
		{
			hr = CreateWICTextureFromFile(&mDevice, widePath, &pResource, shaderResourceViewGPU.GetAddressOf());
		}

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreateDDSTextureFromFile");

			ASSERT(false);
		}

		ID3D11Texture2D* const pTexture2D = reinterpret_cast<ID3D11Texture2D*>(pResource);
		ASSERT(pTexture2D != nullptr);

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		pTexture2D->GetDesc(&textureDesc);

		Texture* const pTexture = new Texture(path, shaderResourceViewGPU, textureDesc.Width, textureDesc.Height);

		mTextureMap.insert(std::make_pair(path, pTexture));

		SafeRelease(pResource);
	}

#undef MAP_ITER
}

Texture* TextureManager::GetTextureOrNull(const std::string& path) const
{
#define MAP_ITER std::unordered_map<std::string, Texture*>::const_iterator

	MAP_ITER iter = mTextureMap.find(path);

	if (iter != mTextureMap.end())
	{
		return iter->second;
	}

#undef MAP_ITER

	return nullptr;
}

void TextureManager::UnloadTexture(const std::string& path)
{
#define MAP_ITER std::unordered_map<std::string, Texture*>::iterator

	MAP_ITER iter = mTextureMap.find(path);

	if (iter != mTextureMap.end())
	{
		delete iter->second;

		mTextureMap.erase(iter);
	}

#undef MAP_ITER
}

void TextureManager::DrawEditorUI()
{
	ImGui::PushID("TextureManager");

	ImGui::Text(UTF8_TEXT("텍스처 목록"));

#define MAP_ITER std::unordered_map<std::string, Texture*>::const_iterator

	for (MAP_ITER iter = mTextureMap.cbegin(); iter != mTextureMap.cend(); ++iter)
	{
		const std::pair<const std::string, Texture*>& pair = *iter;

		Texture* const pTexture = pair.second;
		ASSERT(pTexture != nullptr);

		const char* path = pTexture->GetPath();
		const int width = pTexture->GetWidth();
		const int height = pTexture->GetHeight();

		ImGui::Text("%s (%d x %d)", path, width, height);
	}

#undef MAP_ITER

	ImGui::PopID();
}

void TextureManager::Initialize(ID3D11Device& device)
{
	ASSERT(spInstance == nullptr);

	spInstance = new TextureManager(device);
}