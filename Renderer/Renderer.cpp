#include "Renderer.h"

#include <algorithm>

#include "DDSTextureLoader.h"

#include "CommonDefs.h"
#include "ComHelper.h"
#include "Mesh.h"
#include "Material.h"
#include "Component/MeshComponent.h"
#include "Component/CameraComponent.h"
#include "Actor.h"
#include "DebugSphere.h"

enum
{
	DEFAULT_BUFFER_SIZE = 8
};

Renderer* Renderer::spInstance = nullptr;

Renderer::Renderer(const HWND hWnd)
	: mpDevice(nullptr)
	, mpDeviceContext(nullptr)
	, mpSwapChain(nullptr)
	, mInputLayoutMap{ nullptr, }
	, mVertexShaderMap()
	, mViewport{ 0.f, }
	, mpWireframeState(nullptr)
	, mbWireframe(false)
	, mPixelShaderMap()
	, mpRenderTargetViewGPU(nullptr)
	, mpDepthStencilViewGPU(nullptr)
	, mpBlendState(nullptr)
	, mSamplerStateMap{ nullptr, }
	, mMeshMap()
	, mMaterialMap()
	, mTextureViewMap()
	, mCBFrame{}
	, mpCBFrameGPU(nullptr)
	, mCBWorldMatrix{}
	, mpCBWorldMatrixGPU(nullptr)
	, mErrorCode(S_OK)
	, mWidth(0)
	, mHeight(0)
	, mTargetFrameRate(120)
	, mbVSync(false)
	, mClearColor{ 1.f, 1.f, 1.f, 1.f }
	, mCameraComponents()
	, mSelectedNumber(-1)
	, mpDebugSphere(nullptr)
	, mbDrawDebugSphere(nullptr)
{
	ASSERT(hWnd != nullptr);

	// reserve
	mVertexShaderMap.reserve(DEFAULT_BUFFER_SIZE);
	mPixelShaderMap.reserve(DEFAULT_BUFFER_SIZE);
	mMeshMap.reserve(DEFAULT_BUFFER_SIZE);
	mMaterialMap.reserve(DEFAULT_BUFFER_SIZE);
	mTextureViewMap.reserve(DEFAULT_BUFFER_SIZE);
	mCameraComponents.reserve(DEFAULT_BUFFER_SIZE);

	// d3d
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	sd.BufferCount = 2;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = mTargetFrameRate;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&sd,
		&mpSwapChain,
		&mpDevice,
		nullptr,
		&mpDeviceContext
	);

	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "CreateDeviceAndSwapChain");

		ASSERT(false);

		mErrorCode = hr;
	}

	D3D11_RASTERIZER_DESC wireframeState;
	ZeroMemory(&wireframeState, sizeof(wireframeState));

	wireframeState.FillMode = D3D11_FILL_WIREFRAME;
	wireframeState.CullMode = D3D11_CULL_NONE;
	wireframeState.DepthClipEnable = true;

	hr = mpDevice->CreateRasterizerState(&wireframeState, &mpWireframeState);
	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "CreateRasterizerState");

		ASSERT(false);
	}

	// ui
	IMGUI_CHECKVERSION();
	ImGuiContext* pImGuiContext = ImGui::CreateContext();
	if (pImGuiContext == nullptr)
	{
		LOG_SYSTEM_ERROR(E_FAIL, "ImGui - CreateContext");

		ASSERT(false);
	}

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	if (!ImGui_ImplWin32_Init(hWnd))
	{
		LOG_SYSTEM_ERROR(E_FAIL, "ImGui - Win32_Init");

		ASSERT(false);
	}

	if (!ImGui_ImplDX11_Init(mpDevice, mpDeviceContext))
	{
		LOG_SYSTEM_ERROR(E_FAIL, "ImGui - DX11_Init");

		ASSERT(false);
	}
}

Renderer::~Renderer()
{
	delete mpDebugSphere;

	// ui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// resources
	SafeRelease(mpCBWorldMatrixGPU);
	SafeRelease(mpCBFrameGPU);

	for (std::pair<const std::wstring, ID3D11ShaderResourceView*>& pair : mTextureViewMap)
	{
		SafeRelease(pair.second);
	}

	for (std::pair<const std::wstring, Material*>& pair : mMaterialMap)
	{
		delete pair.second;
	}

	for (std::pair<const std::wstring, Mesh*>& pair : mMeshMap)
	{
		delete pair.second;
	}

	for (ID3D11SamplerState* pSamplerState : mSamplerStateMap)
	{
		SafeRelease(pSamplerState);
	}

	// om
	SafeRelease(mpBlendState);
	SafeRelease(mpDepthStencilViewGPU);
	SafeRelease(mpRenderTargetViewGPU);

	// ps
	for (std::pair<const std::wstring, ID3D11PixelShader*>& pair : mPixelShaderMap)
	{
		SafeRelease(pair.second);
	}

	// rs
	SafeRelease(mpWireframeState);

	// vs
	for (std::pair<const std::wstring, ID3D11VertexShader*>& pair : mVertexShaderMap)
	{
		SafeRelease(pair.second);
	}

	// ia
	for (ID3D11InputLayout* pInputLayout : mInputLayoutMap)
	{
		SafeRelease(pInputLayout);
	}

	SafeRelease(mpSwapChain);
	SafeRelease(mpDeviceContext);
	SafeRelease(mpDevice);
}

void Renderer::UpdateCBFrame(const CBFrame& buffer)
{
	ASSERT(mpDeviceContext != nullptr);

	mpDeviceContext->UpdateSubresource(mpCBFrameGPU, 0, nullptr, &buffer, 0, 0);
}

void Renderer::UpdateCBWorldMatrix(const CBWorldMatrix& buffer)
{
	ASSERT(mpDeviceContext != nullptr);

	mpDeviceContext->UpdateSubresource(mpCBWorldMatrixGPU, 0, nullptr, &buffer, 0, 0);
}

void Renderer::BeginFrame()
{
	ASSERT(mpDeviceContext != nullptr);
	ASSERT(mpSwapChain != nullptr);

	mpDeviceContext->ClearRenderTargetView(mpRenderTargetViewGPU, mClearColor);
	mpDeviceContext->ClearDepthStencilView(mpDepthStencilViewGPU, D3D11_CLEAR_DEPTH, 1.f, 255);

	mpDeviceContext->RSSetViewports(1, &mViewport);

	if (mbWireframe)
	{
		mpDeviceContext->RSSetState(mpWireframeState);
	}
	else
	{
		mpDeviceContext->RSSetState(nullptr);
	}

	mpDeviceContext->OMSetRenderTargets(1, &mpRenderTargetViewGPU, mpDepthStencilViewGPU);
}

void Renderer::RenderScene()
{
	ASSERT(mpDeviceContext != nullptr);

	for (MeshComponent* pMeshComponent : mMeshComponents)
	{
		pMeshComponent->Draw(*mpDeviceContext);
	}

	if (mbDrawDebugSphere)
	{
		mpDebugSphere->Draw(*mpDeviceContext);

		mbDrawDebugSphere = false;
	}
}

void Renderer::BeginUI() const
{
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
}

void Renderer::DrawUI()
{
	ImGui::PushID("Renderer");
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("FPS: %d", static_cast<int>(io.Framerate));

		ImGui::Checkbox("VSync", &mbVSync);
		ImGui::SameLine();
		ImGui::Checkbox("Wireframe(F4)", &mbWireframe);

		constexpr int TARGET_FRAME_RATE_COUNT = 1;

		// TODO: 모니터에 맞는 옵션 선택지 추가
		constexpr const char* const targetFrameRates[TARGET_FRAME_RATE_COUNT] = {
			"120"
		};

		int index = 0;
		ImGui::Combo("FrameRate", reinterpret_cast<int*>(&index), targetFrameRates, TARGET_FRAME_RATE_COUNT);

		ImGui::SliderFloat4("ClearColor", mClearColor, 0.f, 1.f, "%.1f");

		for (int i = 0; i < mCameraComponents.size(); ++i)
		{
			CameraComponent* const pCameraComponent = mCameraComponents[i];

			const Actor* const pOwner = pCameraComponent->GetOwner();

			ImGui::RadioButton(pOwner->GetLabel(), &mSelectedNumber, i);
		}

		if (!mCameraComponents.empty())
		{
			mCameraComponents[mSelectedNumber]->SetActive();
		}
	}
	ImGui::PopID();
}

void Renderer::EndUI() const
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::EndFrame()
{
	ASSERT(mpSwapChain != nullptr);

	mpSwapChain->Present(mbVSync, 0);
}

bool Renderer::TryInitialize(const HWND hWnd)
{
	ASSERT(hWnd != nullptr);
	ASSERT(spInstance == nullptr);

	spInstance = new Renderer(hWnd);

	if (FAILED(spInstance->mErrorCode))
	{
		return false;
	}

	Renderer& renderer = *spInstance;

	// 공유 혹은 기본 리소스들
	bool bResult = renderer.TryCreateVertexShaderAndInputLayout(SHADER_PATH("VSBasic.hlsl"), EVertexType::POS_NORMAL_UV);
	ASSERT(bResult);

	bResult = renderer.TryCreatePixelShader(SHADER_PATH("PSBasic.hlsl"));
	ASSERT(bResult);

	bResult = renderer.TryCreateVertexShaderAndInputLayout(SHADER_PATH("VSBoundingSphere.hlsl"), EVertexType::POS_NORMAL_UV);
	ASSERT(bResult);

	bResult = renderer.TryCreatePixelShader(SHADER_PATH("PSBoundingSphere.hlsl"));
	ASSERT(bResult);

	HRESULT hr = S_OK;
	D3D11_SAMPLER_DESC samplerDescs[static_cast<uint8_t>(ESamplerType::COUNT)] = {
		{
			D3D11_FILTER_MIN_MAG_MIP_LINEAR,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_TEXTURE_ADDRESS_WRAP,
			0,
			0,
			D3D11_COMPARISON_NEVER,
			0,
			0,
			0
		}
	};

	for (int i = 0; i < static_cast<uint8_t>(ESamplerType::COUNT); ++i)
	{
		hr = renderer.mpDevice->CreateSamplerState(&samplerDescs[i], renderer.mSamplerStateMap + i);

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreateSamplerState");

			ASSERT(false);

			renderer.mErrorCode = hr;
		}
	}

	bResult = renderer.TryCreateTextureView(RESOURCE_PATH("Default.dds"));
	ASSERT(bResult);

	bResult = renderer.TryCreateBuffer(EBufferType::CONSTANT, &renderer.mCBFrame, sizeof(CBFrame), 0, renderer.mpCBFrameGPU);
	ASSERT(bResult);

	bResult = renderer.TryCreateBuffer(EBufferType::CONSTANT, &renderer.mCBWorldMatrix, sizeof(CBWorldMatrix), 0, renderer.mpCBWorldMatrixGPU);
	ASSERT(bResult);

	ID3D11Buffer* pConstantBuffers[2] = {
		renderer.mpCBFrameGPU, renderer.mpCBWorldMatrixGPU
	};

	renderer.mpDeviceContext->VSSetConstantBuffers(0, 2, pConstantBuffers);
	renderer.mpDeviceContext->PSSetConstantBuffers(0, 2, pConstantBuffers);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = renderer.mpDevice->CreateBlendState(&blendDesc, &renderer.mpBlendState);
	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "CreateBlendState");

		ASSERT(false);

		renderer.mErrorCode = hr;
	}

	Mesh* const pTriangle = Shape::CreateTriangleAlloc();
	renderer.mMeshMap.insert({ TEXT("Triangle"), pTriangle });

	Mesh* const pCube = Shape::CreateCubeAlloc();
	renderer.mMeshMap.insert({ TEXT("Cube"), pCube });

	Material* const pBasicMaterial = new Material();
	renderer.mMaterialMap.insert({ TEXT("Basic"), pBasicMaterial });

	renderer.mpDebugSphere = new DebugSphere();

	return true;
}

void Renderer::OnResize(const int width, const int height)
{
	ASSERT(width > 0);
	ASSERT(height > 0);
	ASSERT(mpDeviceContext != nullptr);
	ASSERT(mpSwapChain != nullptr);

	mWidth = width;
	mHeight = height;

	mpDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	SafeRelease(mpRenderTargetViewGPU);

	HRESULT hr = mpSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "SwapChain - ResizeBuffers");

		ASSERT(false);

		return;
	}

	ID3D11Texture2D* pBackBuffer = nullptr;
	{
		hr = mpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "SwapChain - GetBuffer");

			ASSERT(false);

			return;
		}

		hr = mpDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mpRenderTargetViewGPU);

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "Device - CreateRenderTargetView");

			ASSERT(false);

			return;
		}
	}
	SafeRelease(pBackBuffer);

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* depthStencilBuffer = nullptr;
	hr = mpDevice->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer);
	{
		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreateTexture2D - DepthStencilBuffer");

			ASSERT(false);

			return;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthBufferDesc.Format = depthBufferDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		hr = mpDevice->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &mpDepthStencilViewGPU);

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreateDepthStencilView");

			ASSERT(false);

			return;
		}
	}
	SafeRelease(depthStencilBuffer);

	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();

	mViewport.TopLeftX = 0.f;
	mViewport.TopLeftY = 0.f;
	mViewport.Width = static_cast<FLOAT>(width);
	mViewport.Height = static_cast<FLOAT>(height);
}

void Renderer::SwitchWireframeMode()
{
	mbWireframe = !mbWireframe;
}

bool Renderer::TryCreateVertexShaderAndInputLayout(const std::wstring& path, const EVertexType type)
{
	ID3DBlob* pShaderBlob = nullptr;
	{
		if (!tryCompileShader(path.c_str(), EShaderType::VERTEX, pShaderBlob))
		{
			return false;
		}

		constexpr D3D11_INPUT_ELEMENT_DESC layoutDescs[3] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, uv), D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		HRESULT hr = S_OK;
		if (mInputLayoutMap[static_cast<uint8_t>(type)] == nullptr)
		{
			hr = mpDevice->CreateInputLayout(
				layoutDescs,
				ARRAYSIZE(layoutDescs),
				pShaderBlob->GetBufferPointer(),
				pShaderBlob->GetBufferSize(),
				mInputLayoutMap + static_cast<uint8_t>(type)
			);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateInputLayout");

				ASSERT(false);

				mErrorCode = hr;

				SafeRelease(pShaderBlob);

				return false;
			}
		}

		ID3D11VertexShader* pVertexShader = nullptr;

		hr = mpDevice->CreateVertexShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&pVertexShader
		);

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreateVertexShader");

			ASSERT(false);

			mErrorCode = hr;

			SafeRelease(pShaderBlob);

			return false;
		}

		mVertexShaderMap.insert({ path, pVertexShader });
	}
	SafeRelease(pShaderBlob);

	return true;
}

bool Renderer::TryCreatePixelShader(const std::wstring& path)
{
	ID3DBlob* pShaderBlob = nullptr;
	{
		if (!tryCompileShader(path.c_str(), EShaderType::PIXEL, pShaderBlob))
		{
			return false;
		}

		ID3D11PixelShader* pPixelShader = nullptr;

		const HRESULT hr = mpDevice->CreatePixelShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&pPixelShader
		);

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreatePixelShader");

			ASSERT(false);

			mErrorCode = hr;

			return false;
		}

		mPixelShaderMap.insert({ path, pPixelShader });
	}

	return true;
}

bool Renderer::TryCreateTextureView(const std::wstring& path)
{
	ID3D11ShaderResourceView* pTextureView = nullptr;

	const HRESULT hr = CreateDDSTextureFromFile(mpDevice, path.c_str(), nullptr, &pTextureView);
	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "CreateDDSTextureFromFile");

		ASSERT(false);

		mErrorCode = hr;

		return false;
	}

	mTextureViewMap.insert({ path, pTextureView });

	return true;
}

ID3D11VertexShader* Renderer::GetVertexShaderOrNull(const std::wstring& path) const
{
#define MAP_ITER std::unordered_map<std::wstring, ID3D11VertexShader*>::const_iterator

	const MAP_ITER iter = mVertexShaderMap.find(path);

	if (iter != mVertexShaderMap.cend())
	{
		return iter->second;
	}

#undef MAP_ITER

	return nullptr;
}

ID3D11PixelShader* Renderer::GetPixelShaderOrNull(const std::wstring& path) const
{
#define MAP_ITER std::unordered_map<std::wstring, ID3D11PixelShader*>::const_iterator

	const MAP_ITER iter = mPixelShaderMap.find(path);

	if (iter != mPixelShaderMap.cend())
	{
		return iter->second;
	}

#undef MAP_ITER

	return nullptr;
}

Mesh* Renderer::GetMeshOrNull(const std::wstring& path) const
{
#define MAP_ITER std::unordered_map<std::wstring, Mesh*>::const_iterator

	const MAP_ITER iter = mMeshMap.find(path);
	if (iter != mMeshMap.cend())
	{
		return iter->second;
	}

#undef MAP_ITER

	return nullptr;
}

Material* Renderer::GetMaterialOrNull(const std::wstring& path) const
{
#define MAP_ITER std::unordered_map<std::wstring, Material*>::const_iterator

	const MAP_ITER iter = mMaterialMap.find(path);
	if (iter != mMaterialMap.cend())
	{
		return iter->second;
	}

#undef MAP_ITER

	return nullptr;
}

ID3D11ShaderResourceView* Renderer::GetTextureViewOrNull(const std::wstring& path) const
{
#define MAP_ITER std::unordered_map<std::wstring, ID3D11ShaderResourceView*>::const_iterator

	const MAP_ITER iter = mTextureViewMap.find(path);

	if (iter != mTextureViewMap.cend())
	{
		return iter->second;
	}

#undef MAP_ITER

	return nullptr;
}

ID3D11BlendState* Renderer::GetBlendState() const
{
	return mpBlendState;
}

bool Renderer::TryCreateBuffer(
	const EBufferType type,
	const void* const pData,
	const UINT byteSize,
	const UINT stride,
	ID3D11Buffer*& pOutBuffer
) const
{
	ASSERT(static_cast<uint8_t>(type) < static_cast<uint8_t>(EBufferType::COUNT));
	ASSERT(pData != nullptr);
	ASSERT(byteSize > 0);

	constexpr UINT BIND_FLAGS[static_cast<uint8_t>(EBufferType::COUNT)] = {
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_BIND_INDEX_BUFFER,
		D3D11_BIND_CONSTANT_BUFFER
	};

	constexpr D3D11_USAGE USAGE_FLAGS[static_cast<uint8_t>(EBufferType::COUNT)] = {
		D3D11_USAGE_IMMUTABLE,
		D3D11_USAGE_IMMUTABLE,
		D3D11_USAGE_DEFAULT
	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.ByteWidth = byteSize;
	bufferDesc.Usage = USAGE_FLAGS[static_cast<uint8_t>(type)];
	bufferDesc.BindFlags = BIND_FLAGS[static_cast<uint8_t>(type)];
	bufferDesc.StructureByteStride = stride;

	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(bufferData));

	bufferData.pSysMem = pData;

	const HRESULT hr = mpDevice->CreateBuffer(&bufferDesc, &bufferData, &pOutBuffer);

	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "CreateBuffer");

		ASSERT(false);

		return false;
	}

	return true;
}

void Renderer::AddMeshComponent(MeshComponent* pMeshComponent)
{
	ASSERT(pMeshComponent != nullptr);

	mMeshComponents.push_back(pMeshComponent);
}

void Renderer::RemoveMeshComponent(MeshComponent* pMeshComponent)
{
	ASSERT(pMeshComponent != nullptr);

#define VECTOR_ITER std::vector<MeshComponent*>::iterator

	VECTOR_ITER iter = std::find(mMeshComponents.begin(), mMeshComponents.end(), pMeshComponent);

	mMeshComponents.erase(iter);

#undef VECTOR_ITER
}

void Renderer::AddCameraComponent(CameraComponent* const pCameraComponent)
{
	ASSERT(pCameraComponent != nullptr);

	if (mCameraComponents.empty())
	{
		mSelectedNumber = 0;
	}

	mCameraComponents.push_back(pCameraComponent);
}

void Renderer::RemoveCameraComponent(CameraComponent* const pCameraComponent)
{
	ASSERT(pCameraComponent != nullptr);

#define VECTOR_ITER std::vector<CameraComponent*>::iterator

	VECTOR_ITER iter = std::find(mCameraComponents.begin(), mCameraComponents.end(), pCameraComponent);

	if (iter != mCameraComponents.end())
	{
		mCameraComponents.erase(iter);
	}

#undef VECTOR_ITER
}

void Renderer::SetDebugSphere(const Vector3 center, const float radius)
{
	mpDebugSphere->SetCenter(center);
	mpDebugSphere->SetScale(radius);

	mbDrawDebugSphere = true;
}

bool Renderer::TryGetMouseRay(const Vector2 mousePos, Ray& ray) const
{
	if (mCameraComponents.empty())
	{
		return false;
	}

	const Vector2 screenSize(static_cast<float>(mWidth), static_cast<float>(mHeight));

	Vector2 ndcXY = 2.f * mousePos / screenSize - Vector2(1.f, 1.f);
	ndcXY.y *= -1.f;

	const CameraComponent& cameraComponent = *mCameraComponents[mSelectedNumber];
	const Matrix viewProj = cameraComponent.GetViewProjectionMatrix();

	const Matrix inversedViewProj = viewProj.Invert();

	Vector3 startPos(ndcXY.x, ndcXY.y, 0.f);
	startPos = Vector3::Transform(startPos, inversedViewProj);

	Vector3 endPos(ndcXY.x, ndcXY.y, 1.f);
	endPos = Vector3::Transform(endPos, inversedViewProj);

	Vector3 dir = endPos - startPos;
	dir.Normalize();

	ray.position = startPos;
	ray.direction = dir;

	return true;
}

bool Renderer::tryCompileShader(const TCHAR* const path, const EShaderType type, ID3DBlob*& pOutShaderBlob)
{
	ASSERT(path != nullptr);
	ASSERT(static_cast<uint8_t>(type) < static_cast<uint8_t>(EShaderType::COUNT));

	constexpr const char* const targets[static_cast<uint8_t>(EShaderType::COUNT)] = {
		"vs_5_0",
		"ps_5_0"
	};

	ID3DBlob* pErrorBlob = nullptr;
	{
		UINT shaderCompileFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		shaderCompileFlags |= D3DCOMPILE_DEBUG;
		shaderCompileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		const HRESULT hr = D3DCompileFromFile(
			path,
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			targets[static_cast<int>(type)],
			shaderCompileFlags,
			0,
			&pOutShaderBlob,
			&pErrorBlob
		);

		if (FAILED(hr))
		{
			if (pErrorBlob != nullptr)
			{
				LOG_SYSTEM_ERROR(hr, reinterpret_cast<char*>(pErrorBlob->GetBufferPointer()));
			}
			else
			{
				LOG_SYSTEM_ERROR(hr, "D3DCompileFromFile");
			}

			ASSERT(false);

			mErrorCode = hr;

			return false;
		}
	}
	SafeRelease(pErrorBlob);

	return true;
}
