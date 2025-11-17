#include "ShaderManager.h"

#include <d3dcompiler.h>

#include "Core/ComHelper.h"
#include "Core/StringHelper.h"
#include "Core/LogHelper.h"
#include "UI/ImGuiHeaders.h"
#include "Core/CommonDefs.h"

enum
{
	DEFAULT_BUFFER_SIZE = 32
};

ShaderManager* ShaderManager::spInstance = nullptr;

ShaderManager::ShaderManager(ID3D11Device& device)
	: mDevice(device)
	, mpInputLayout{ nullptr, }
	, mVertexShaderMap()
	, mPixelShaderMap()
{
	mVertexShaderMap.reserve(DEFAULT_BUFFER_SIZE);
	mPixelShaderMap.reserve(DEFAULT_BUFFER_SIZE);

	// vs entry
	const std::pair<const char*, Vertex::EType> vertexShaderEntries[] =
	{
		{ SHADER_PATH("VSBasic.hlsl"), Vertex::EType::POS_NORMAL_UV },
		{ SHADER_PATH("VSSprite.hlsl"), Vertex::EType::POS_NORMAL_UV },
		{ SHADER_PATH("VSFullScreen.hlsl"), Vertex::EType::POS_NORMAL_UV },
		{ SHADER_PATH("VSBlinnPhong.hlsl"), Vertex::EType::POS_NORMAL_UV },
	};

	for (const std::pair<const char*, Vertex::EType>& entry : vertexShaderEntries)
	{
		LoadVertexShaderAndInputLayout(entry.first, entry.second);
	}

	// ps entry
	const char* const pixelShaderEntries[] =
	{
		SHADER_PATH("PSBasic.hlsl"),
		SHADER_PATH("PSSprite.hlsl"),
		SHADER_PATH("PSFullScreen.hlsl"),
		SHADER_PATH("PSDebugSphere.hlsl"),
		SHADER_PATH("PSBlinnPhong.hlsl"),
	};

	for (const char* const entry : pixelShaderEntries)
	{
		LoadPixelShader(entry);
	}
}

ShaderManager::~ShaderManager()
{
	for (std::pair<const std::string, ID3D11PixelShader*>& pair : mPixelShaderMap)
	{
		SafeRelease(pair.second);
	}

	for (std::pair<const std::string, ID3D11VertexShader*>& pair : mVertexShaderMap)
	{
		SafeRelease(pair.second);
	}

	for (ID3D11InputLayout* pInputLayout : mpInputLayout)
	{
		SafeRelease(pInputLayout);
	}
}

void ShaderManager::LoadVertexShaderAndInputLayout(const std::string& path, const Vertex::EType eType)
{
	ID3DBlob* pVSBlob = compileShaderAlloc(path, EShaderType::VERTEX);
	{
		ASSERT(pVSBlob != nullptr);

		ID3D11VertexShader* pVertexShader = nullptr;
		{
			const HRESULT hr = mDevice.CreateVertexShader(
				pVSBlob->GetBufferPointer(),
				pVSBlob->GetBufferSize(),
				nullptr,
				&pVertexShader
			);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateVertexShader");

				ASSERT(false);
			}
		}

		mVertexShaderMap.insert(std::make_pair(path, pVertexShader));

		if (mpInputLayout[static_cast<int>(eType)] == nullptr)
		{
			const std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescs = Vertex::GetInputLayoutDescs(eType);

			ID3D11InputLayout* pInputLayout = nullptr;
			{
				const HRESULT hr = mDevice.CreateInputLayout(
					inputDescs.data(),
					static_cast<UINT>(inputDescs.size()),
					pVSBlob->GetBufferPointer(),
					pVSBlob->GetBufferSize(),
					&pInputLayout
				);

				if (FAILED(hr))
				{
					LOG_SYSTEM_ERROR(hr, "CreateInputLayout");

					ASSERT(false);
				}
			}

			mpInputLayout[static_cast<int>(eType)] = pInputLayout;
		}
	}
	SafeRelease(pVSBlob);
}

ID3D11InputLayout* ShaderManager::GetInputLayoutOrNull(const Vertex::EType eType) const
{
	return mpInputLayout[static_cast<int>(eType)];
}

ID3D11VertexShader* ShaderManager::GetVertexShaderOrNull(const std::string& path) const
{
#define MAP_ITER std::unordered_map<std::string, ID3D11VertexShader*>::const_iterator

	MAP_ITER iter = mVertexShaderMap.find(path);

	if (iter != mVertexShaderMap.end())
	{
		return iter->second;
	}

#undef MAP_ITER

	return nullptr;
}

void ShaderManager::LoadPixelShader(const std::string& path)
{
	ID3DBlob* pPSBlob = compileShaderAlloc(path, EShaderType::PIXEL);
	{
		ASSERT(pPSBlob != nullptr);

		ID3D11PixelShader* pPixelShader = nullptr;
		{
			const HRESULT hr = mDevice.CreatePixelShader(
				pPSBlob->GetBufferPointer(),
				pPSBlob->GetBufferSize(),
				nullptr,
				&pPixelShader
			);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreatePixelShader");

				ASSERT(false);
			}
		}

		mPixelShaderMap.insert(std::make_pair(path, pPixelShader));
	}
	SafeRelease(pPSBlob);
}

ID3D11PixelShader* ShaderManager::GetPixelShaderOrNull(const std::string& path) const
{
#define MAP_ITER std::unordered_map<std::string, ID3D11PixelShader*>::const_iterator

	MAP_ITER iter = mPixelShaderMap.find(path);

	if (iter != mPixelShaderMap.end())
	{
		return iter->second;
	}

#undef MAP_ITER

	return nullptr;
}

void ShaderManager::DrawEditorUI()
{
	ImGui::PushID("ShaderManager");

	ImGui::Text(UTF8_TEXT("Á¤Á¡ ¼ÎÀÌ´õ"));

#define MAP_ITER std::unordered_map<std::string, ID3D11VertexShader*>::const_iterator

	for (MAP_ITER iter = mVertexShaderMap.cbegin(); iter != mVertexShaderMap.cend(); ++iter)
	{
		const std::pair<const std::string, ID3D11VertexShader*>& pair = *iter;
		ImGui::Text("%s", pair.first.c_str());
	}

#undef MAP_ITER

	ImGui::Separator();

	ImGui::Text(UTF8_TEXT("ÇÈ¼¿ ¼ÎÀÌ´õ"));

#define MAP_ITER std::unordered_map<std::string, ID3D11PixelShader*>::const_iterator

	for (MAP_ITER iter = mPixelShaderMap.cbegin(); iter != mPixelShaderMap.cend(); ++iter)
	{
		const std::pair<const std::string, ID3D11PixelShader*>& pair = *iter;
		ImGui::Text("%s", pair.first.c_str());
	}

#undef MAP_ITER

	ImGui::PopID();
}

bool ShaderManager::DrawShaderSelectorPopupAndSelectShaders(std::string& outPath, const bool bPixel)
{
	ImGui::PushID("ShaderManager");

	const char* const popupName = UTF8_TEXT("¼ÎÀÌ´õ ¸ñ·Ï");

	ImGui::OpenPopup(popupName);

	// selecet or cancel -> true
	bool bRet = false;

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (bPixel)
		{
			for (const std::pair<const std::string, ID3D11PixelShader*>& pair : mPixelShaderMap)
			{
				if (ImGui::Button(pair.first.c_str()))
				{
					outPath = pair.first;

					bRet = true;
				}
			}
		}
		else
		{
			for (const std::pair<const std::string, ID3D11VertexShader*>& pair : mVertexShaderMap)
			{
				if (ImGui::Button(pair.first.c_str()))
				{
					outPath = pair.first;

					bRet = true;
				}
			}
		}

		ImGui::EndPopup();
	}

	ImGui::PopID();

	return bRet || ImGui::IsKeyPressed(ImGuiKey_Escape);
}

void ShaderManager::Initialize(ID3D11Device& device)
{
	ASSERT(spInstance == nullptr);

	spInstance = new ShaderManager(device);
}

ID3DBlob* ShaderManager::compileShaderAlloc(const std::string& path, const EShaderType eType) const
{
	constexpr const char* const targets[static_cast<int>(EShaderType::COUNT)] =
	{
		#define SHADER_ENTRY(type, target) target,
		SHADER_LIST
		#undef SHADER_ENTRY
	};

	ID3DBlob* pRetBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;
	{
		UINT shaderCompileFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		shaderCompileFlags |= D3DCOMPILE_DEBUG;
		shaderCompileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		TCHAR widePath[MAX_PATH];
		ConvertMultiToWide(widePath, path.c_str());

		const HRESULT hr = D3DCompileFromFile(
			widePath,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			targets[static_cast<int>(eType)],
			shaderCompileFlags,
			0,
			&pRetBlob,
			&pErrorBlob
		);

		if (FAILED(hr))
		{
			if (pErrorBlob != nullptr)
			{
				LOG_SYSTEM_ERROR(hr, reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			}
			else
			{
				LOG_SYSTEM_ERROR(hr, "D3DCompileFromFile");
			}

			ASSERT(false);
		}
	}
	SafeRelease(pErrorBlob);

	return pRetBlob;
}