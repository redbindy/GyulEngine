#include "Texture.h"

#include <d3d11.h>

#include "Core/Assert.h"

Texture::Texture(
	const std::string& path,
	ComPtr<ID3D11ShaderResourceView>& pTextureViewGPU,
	const int width,
	const int height
)
	: mPath(path)
	, mpTextureViewGPU(pTextureViewGPU)
	, mWidth(width)
	, mHeight(height)
{
	ASSERT(pTextureViewGPU != nullptr);
	ASSERT(width > 0);
	ASSERT(height > 0);
}

void Texture::Bind(ID3D11DeviceContext& deviceContext)
{
	deviceContext.PSSetShaderResources(0, 1, mpTextureViewGPU.GetAddressOf());
}
