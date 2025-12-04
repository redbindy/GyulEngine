#include "Texture.h"

Texture::Texture(const std::wstring& name)
	: Resource(name)
	, mbAlpha(false)
	, mTextureType(ETextureType::NONE)
	, mWidth(0)
	, mHeight(0)
{
}

Texture::Texture(
	const std::wstring& name,
	const uint32_t width,
	const uint32_t height
)
	: Resource(name)
	, mbAlpha(false)
	, mTextureType(ETextureType::NONE)
	, mWidth(width)
	, mHeight(height)
{
	// create empty texture with width and height
}

HRESULT Texture::Load(const std::wstring& path)
{
	return E_NOTIMPL;
}
