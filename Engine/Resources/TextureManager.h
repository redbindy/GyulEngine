#pragma once

#include <unordered_map>

#include <d3d11.h>

#include "Core/Assert.h"
#include "UI/IEditorUIDrawable.h"

class Texture;

class TextureManager final : public IEditorUIDrawable
{
public:
	void LoadTexture(const std::string& path);
	Texture* GetTextureOrNull(const std::string& path) const;
	void UnloadTexture(const std::string& path);

	virtual void DrawEditorUI() override;

	// static
	static void Initialize(ID3D11Device& device);

	static TextureManager& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	static TextureManager* spInstance;

	ID3D11Device& mDevice;

	std::unordered_map<std::string, Texture*> mTextureMap;

private:
	TextureManager(ID3D11Device& device);
	~TextureManager();

private:
	TextureManager(const TextureManager& other) = delete;
	TextureManager(TextureManager&& other) = delete;
	TextureManager& operator=(const TextureManager& other) = delete;
	TextureManager& operator=(TextureManager&& other) = delete;
};