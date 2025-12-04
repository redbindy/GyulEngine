#pragma once

#include <cstdint>

#include "Resource.h"

class Texture final : public Resource
{
public:
	enum class ETextureType
	{
		NONE,
		BMP,
		PNG,
	};

public:
	Texture(const std::wstring& name);
	Texture(
		const std::wstring& name,
		const uint32_t width,
		const uint32_t height
	);
	virtual ~Texture() = default;

	virtual HRESULT Load(const std::wstring& path) override;

	virtual EResourceType GetResourceType() const override
	{
		return EResourceType::TEXTURE;
	}

	uint32_t GetWidth() const
	{
		return mWidth;
	}

	void SetWidth(const uint32_t width)
	{
		mWidth = width;
	}

	uint32_t GetHeight() const
	{
		return mHeight;
	}

	void SetHeight(const uint32_t height)
	{
		mHeight = height;
	}

private:
	bool mbAlpha;

	ETextureType mTextureType;

	uint32_t mWidth;
	uint32_t mHeight;
};