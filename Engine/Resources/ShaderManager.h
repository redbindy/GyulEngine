#pragma once

#include <unordered_map>

#include <d3d11.h>

#include "Core/Assert.h"
#include "Renderer/Vertex.h"

#define SHADER_LIST \
	SHADER_ENTRY(VERTEX, "vs_5_0") \
	SHADER_ENTRY(PIXEL, "ps_5_0") \

#define SHADER_PATH(filename) ("./Shaders/" filename)

class ShaderManager final
{
public:
	void LoadVertexShaderAndInputLayout(const std::string& path, const Vertex::EType eType);
	ID3D11InputLayout* GetInputLayoutOrNull(const Vertex::EType eType) const;
	ID3D11VertexShader* GetVertexShaderOrNull(const std::string& path) const;

	void LoadPixelShader(const std::string& path);
	ID3D11PixelShader* GetPixelShaderOrNull(const std::string& path) const;

	// static
	static void Initialize(ID3D11Device& device);

	static ShaderManager& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static void Destroy()
	{
		delete spInstance;
	}

private:
	enum class EShaderType
	{
#define SHADER_ENTRY(type, target) type,
		SHADER_LIST
#undef SHADER_ENTRY

		COUNT
	};

private:
	static ShaderManager* spInstance;

	ID3D11Device& mDevice;

	ID3D11InputLayout* mpInputLayout[Vertex::GetVertexTypeCount()];
	std::unordered_map<std::string, ID3D11VertexShader*> mVertexShaderMap;
	std::unordered_map<std::string, ID3D11PixelShader*> mPixelShaderMap;

private:
	ShaderManager(ID3D11Device& device);
	~ShaderManager();

	ID3DBlob* compileShaderAlloc(const std::string& path, const EShaderType eType) const;

private:
	ShaderManager(const ShaderManager& other) = delete;
	ShaderManager(ShaderManager&& other) = delete;
	ShaderManager& operator=(const ShaderManager& other) = delete;
	ShaderManager& operator=(ShaderManager&& other) = delete;
};