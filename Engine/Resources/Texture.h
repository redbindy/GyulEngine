#pragma once

#include <string>

#include "Core/ComHelper.h"

struct ID3D11ShaderResourceView;
struct ID3D11DeviceContext;

class Texture final
{
public:
	Texture(
		const std::string& path,
		ComPtr<ID3D11ShaderResourceView>& pTextureViewGPU,
		const int width,
		const int height
	);
	~Texture() = default;

	void Bind(ID3D11DeviceContext& deviceContext);

	inline const char* GetPath() const
	{
		return mPath.c_str();
	}

	inline int GetWidth() const
	{
		return mWidth;
	}

	inline int GetHeight() const
	{
		return mHeight;
	}

private:
	std::string mPath;

	ComPtr<ID3D11ShaderResourceView> mpTextureViewGPU;

	int mWidth;
	int mHeight;

private:
	Texture(const Texture& other) = delete;
	Texture& operator=(const Texture& other) = delete;
	Texture(Texture&& other) = delete;
	Texture& operator=(Texture&& other) = delete;
};