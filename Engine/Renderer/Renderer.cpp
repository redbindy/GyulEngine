#include "Renderer.h"

#include "UI/ImGuiHeaders.h"

#include "Core/ComHelper.h"
#include "Core/LogHelper.h"
#include "Resources/TextureManager.h"
#include "Resources/ShaderManager.h"
#include "Resources/MeshManager.h"
#include "Resources/MaterialManager.h"
#include "Resources/ModelManager.h"

#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include "Core/CommonDefs.h"

#include "Scene/Components/CameraComponent.h"
#include "Scene/Components/MeshComponent.h"

enum
{
	DEFAULT_COMMAND_QUEUE_SIZE = 256,
	DEFAULT_BUFFER_SIZE = 32
};

// 구조체에 대해서 초기화 경고 끄기
#pragma warning(push)
#pragma warning(disable : 26495)
struct CBFrame
{
	Vector3 cameraPos;
	float dummy;
	Matrix viewProj;
};

struct CBWorldMatrix
{
	Matrix world;
	Matrix invTrans;
};
#pragma warning(pop)

Renderer* Renderer::spInstance = nullptr;

Renderer::Renderer(
	ID3D11Device* const pDevice,
	ID3D11DeviceContext* const pDeviceContext,
	IDXGISwapChain1* const pSwapChain,
	const UINT refreshRate
)
	: mpDevice(pDevice)
	, mpDeviceContext(pDeviceContext)
	, mpSwapChain(pSwapChain)
	, mpBackBufferViewGPU(nullptr)
	, mpMultiSampleRenderTargetViewGPU(nullptr)
	, mpMultiSampleDepthStencilViewGPU(nullptr)
	, mpHDRRenderTargetViewGPU(nullptr)
	, mpHDRResourceViewGPU(nullptr)
	, mpSDRRenderTargetViewGPU(nullptr)
	, mpCommonDepthStencilViewGPU(nullptr)
	, mViewport{ 0.f, }
	, mRefreshRate(refreshRate)
	, mbVSync(false)
	, mClearColor{ 1.f, 1.f, 1.f, 1.f }
	, mRenderCommandQueue()
	, mpCBFrameGPU(nullptr)
	, mpCBWorldMatrixGPU(nullptr)
	, mpEditorCameraComponent(nullptr)
	, mpMainCameraComponent(nullptr)
	, mSceneComponents()
	, mDebugSphereRenderCommand{}
	, mbOnDebugSphere(false)
	, mLightPool{}
	, mLightCount(0)
{
	ASSERT(pDevice != nullptr);
	ASSERT(pDeviceContext != nullptr);
	ASSERT(pSwapChain != nullptr);

	mRenderCommandQueue.reserve(DEFAULT_COMMAND_QUEUE_SIZE);
	mSceneComponents.reserve(DEFAULT_BUFFER_SIZE);

	// -----------------------------
	// Pipeline state objects create
	// -----------------------------
	// Rasterizer States
	{
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));

		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_BACK;
		rd.DepthClipEnable = TRUE;

		ID3D11RasterizerState* pRS = nullptr;
		HRESULT hr = mpDevice->CreateRasterizerState(&rd, &pRS);
		ASSERT(SUCCEEDED(hr));

		mRasterizerStateMap[ERasterizerType::SOLID] = pRS;

		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.CullMode = D3D11_CULL_NONE;

		pRS = nullptr;
		hr = mpDevice->CreateRasterizerState(&rd, &pRS);
		ASSERT(SUCCEEDED(hr));

		mRasterizerStateMap[ERasterizerType::WIREFRAME] = pRS;
	}

	// Sampler States
	{
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));

		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MaxLOD = D3D11_FLOAT32_MAX;

		ID3D11SamplerState* pSampler = nullptr;
		HRESULT hr = mpDevice->CreateSamplerState(&sd, &pSampler);
		ASSERT(SUCCEEDED(hr));

		mSamplerStateMap[ESamplerType::LINEAR_WRAP] = pSampler;
	}

	// Blend States
	{
		// Opaque
		D3D11_BLEND_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.AlphaToCoverageEnable = FALSE;
		bd.IndependentBlendEnable = FALSE;

		D3D11_RENDER_TARGET_BLEND_DESC& rtb = bd.RenderTarget[0];

		rtb.BlendEnable = FALSE;
		rtb.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		ID3D11BlendState* pBlend = nullptr;
		HRESULT hr = mpDevice->CreateBlendState(&bd, &pBlend);
		ASSERT(SUCCEEDED(hr));

		mBlendStateMap[EBlendStateType::OPAQUE] = pBlend;

		// Alpha blend (standard)
		ZeroMemory(&bd, sizeof(bd));

		rtb = bd.RenderTarget[0];
		bd.IndependentBlendEnable = FALSE;
		rtb.BlendEnable = TRUE;
		rtb.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtb.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtb.BlendOp = D3D11_BLEND_OP_ADD;
		rtb.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtb.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtb.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtb.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		pBlend = nullptr;
		hr = mpDevice->CreateBlendState(&bd, &pBlend);
		ASSERT(SUCCEEDED(hr));

		mBlendStateMap[EBlendStateType::ALPHA_BLEND] = pBlend;

		// Additive
		ZeroMemory(&bd, sizeof(bd));

		rtb = bd.RenderTarget[0];
		bd.IndependentBlendEnable = FALSE;
		rtb.BlendEnable = TRUE;
		rtb.SrcBlend = D3D11_BLEND_ONE;
		rtb.DestBlend = D3D11_BLEND_ONE;
		rtb.BlendOp = D3D11_BLEND_OP_ADD;
		rtb.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtb.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtb.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtb.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		pBlend = nullptr;
		hr = mpDevice->CreateBlendState(&bd, &pBlend);
		ASSERT(SUCCEEDED(hr));

		mBlendStateMap[EBlendStateType::ADDITIVE] = pBlend;
	}

	// Depth Stencil States
	{
		// Enabled
		D3D11_DEPTH_STENCIL_DESC dd;
		ZeroMemory(&dd, sizeof(dd));

		dd.DepthEnable = TRUE;
		dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dd.DepthFunc = D3D11_COMPARISON_LESS;
		dd.StencilEnable = FALSE;

		ID3D11DepthStencilState* pDS = nullptr;
		HRESULT hr = mpDevice->CreateDepthStencilState(&dd, &pDS);
		ASSERT(SUCCEEDED(hr));

		mDepthStencilStateMap[EDepthStencilType::DEPTH_ENABLED] = pDS;

		// Disabled
		ZeroMemory(&dd, sizeof(dd));

		dd.DepthEnable = FALSE;
		dd.StencilEnable = FALSE;

		pDS = nullptr;
		hr = mpDevice->CreateDepthStencilState(&dd, &pDS);
		ASSERT(SUCCEEDED(hr));

		mDepthStencilStateMap[EDepthStencilType::DEPTH_DISABLED] = pDS;
	}

	// CBFrame
	{
		CBFrame cbFrame;
		cbFrame.cameraPos = Vector3(0.f, 0.f, 0.f);
		cbFrame.viewProj = Matrix::Identity;

		mpCBFrameGPU = createConstantBufferAlloc(&cbFrame, sizeof(CBFrame));
	}

	// world matrix
	{
		CBWorldMatrix cbWorld;
		cbWorld.world = Matrix::Identity;
		cbWorld.invTrans = Matrix::Identity;

		mpCBWorldMatrixGPU = createConstantBufferAlloc(&cbWorld, sizeof(CBWorldMatrix));
	}

	ID3D11Buffer* const defaultBuffers[] = {
		mpCBFrameGPU, mpCBWorldMatrixGPU
	};

	constexpr int ARRAY_SIZE = ARRAYSIZE(defaultBuffers);

	mpDeviceContext->VSSetConstantBuffers(0, ARRAY_SIZE, defaultBuffers);
	mpDeviceContext->PSSetConstantBuffers(0, ARRAY_SIZE, defaultBuffers);

	// debug sphere
	{
		MeshManager& meshManager = MeshManager::GetInstance();
		MaterialManager& materialManager = MaterialManager::GetInstance();

		mDebugSphereRenderCommand.pMesh = meshManager.GetMeshOrNull("Sphere");
		mDebugSphereRenderCommand.pMaterial = materialManager.GetMaterialOrNull("DebugSphere");
		mDebugSphereRenderCommand.worldMatrix = Matrix::Identity;
	}

	// light
	{
		mpCBLightGPU = createConstantBufferAlloc(mLightPool, sizeof(mLightPool));

		mpDeviceContext->PSSetConstantBuffers(CB_LIGHT_SLOT, 1, &mpCBLightGPU);
	}
}

Renderer::~Renderer()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SafeRelease(mpCBLightGPU);
	SafeRelease(mpCBWorldMatrixGPU);
	SafeRelease(mpCBFrameGPU);

	for (std::pair<const EDepthStencilType, ID3D11DepthStencilState*>& pair : mDepthStencilStateMap)
	{
		SafeRelease(pair.second);
	}

	for (std::pair<const EBlendStateType, ID3D11BlendState*>& pair : mBlendStateMap)
	{
		SafeRelease(pair.second);
	}

	for (std::pair<const ESamplerType, ID3D11SamplerState*>& pair : mSamplerStateMap)
	{
		SafeRelease(pair.second);
	}

	for (std::pair<const ERasterizerType, ID3D11RasterizerState*>& pair : mRasterizerStateMap)
	{
		SafeRelease(pair.second);
	}

	ModelManager::Destroy();
	MaterialManager::Destroy();
	MeshManager::Destroy();
	ShaderManager::Destroy();
	TextureManager::Destroy();

	SafeRelease(mpCommonDepthStencilViewGPU);
	SafeRelease(mpSDRRenderTargetViewGPU);

	SafeRelease(mpHDRResourceViewGPU);
	SafeRelease(mpHDRRenderTargetViewGPU);

	SafeRelease(mpMultiSampleDepthStencilViewGPU);
	SafeRelease(mpMultiSampleRenderTargetViewGPU);

	SafeRelease(mpBackBufferViewGPU);

	SafeRelease(mpSwapChain);
	SafeRelease(mpDeviceContext);
	SafeRelease(mpDevice);
}

ID3D11Buffer* Renderer::createConstantBufferAlloc(const void* const pData, const UINT byteWidth)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = byteWidth;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA initialData;
	ZeroMemory(&initialData, sizeof(D3D11_SUBRESOURCE_DATA));

	initialData.pSysMem = pData;

	ID3D11Buffer* pRet = nullptr;
	HRESULT hr = mpDevice->CreateBuffer(&bufferDesc, &initialData, &pRet);
	ASSERT(SUCCEEDED(hr));

	return pRet;
}

void Renderer::BeginFrame() const
{
	mpDeviceContext->RSSetViewports(1, &mViewport);

	if (mbMultiSampling)
	{
		mpDeviceContext->ClearRenderTargetView(mpMultiSampleRenderTargetViewGPU, mClearColor);
		mpDeviceContext->ClearDepthStencilView(mpMultiSampleDepthStencilViewGPU, D3D11_CLEAR_DEPTH, 1.f, 0xFF);

		mpDeviceContext->OMSetRenderTargets(1, &mpMultiSampleRenderTargetViewGPU, mpMultiSampleDepthStencilViewGPU);
	}
	else
	{
		mpDeviceContext->ClearRenderTargetView(mpHDRRenderTargetViewGPU, mClearColor);
		mpDeviceContext->ClearDepthStencilView(mpCommonDepthStencilViewGPU, D3D11_CLEAR_DEPTH, 1.f, 0xFF);

		mpDeviceContext->OMSetRenderTargets(1, &mpHDRRenderTargetViewGPU, mpCommonDepthStencilViewGPU);
	}
}

void Renderer::EndFrame() const
{
	mpSwapChain->Present(mbVSync, 0);
}

void Renderer::RenderScene(const std::string& sceneName)
{
	mpMainCameraComponent->UpdateCameraInfomation();

	mpDeviceContext->UpdateSubresource(
		mpCBLightGPU,
		0,
		nullptr,
		mLightPool,
		0,
		0
	);

	memset(mLightPool, 0, sizeof(mLightPool));
	mLightCount = 0;

	// frustum culling
	const std::vector<MeshComponent*>& meshComponentList = mSceneComponents[sceneName];

	for (MeshComponent* const pMeshComponent : meshComponentList)
	{
		const BoundingSphere& boundingSphereWorld = pMeshComponent->GetBoundingSphereWorld();

		if (!mbViewFrustumCulling || mpMainCameraComponent->IsInViewFrustum(boundingSphereWorld))
		{
			pMeshComponent->SubmitRenderCommand();
		}
	}

	// draw call
	for (const RenderCommand& command : mRenderCommandQueue)
	{
		command.pMesh->Bind(*mpDeviceContext);
		command.pMaterial->Bind(*mpDeviceContext);

		const CBWorldMatrix cbWorldMat =
		{
			command.worldMatrix.Transpose(),
			command.worldMatrix.Invert() // hlsl이 col-major라 전치 생략
		};

		mpDeviceContext->UpdateSubresource(
			mpCBWorldMatrixGPU,
			0,
			nullptr,
			&cbWorldMat,
			0,
			0
		);

		if (mbWireframeMode)
		{
			mpDeviceContext->RSSetState(mRasterizerStateMap[ERasterizerType::WIREFRAME]);
		}

		mpDeviceContext->DrawIndexed(
			command.pMesh->GetIndexCount(),
			0,
			0
		);
	}
	mRenderCommandQueue.clear();

	if (mbOnDebugSphere)
	{
		mDebugSphereRenderCommand.pMesh->Bind(*mpDeviceContext);
		mDebugSphereRenderCommand.pMaterial->Bind(*mpDeviceContext);

		const CBWorldMatrix cbWorldMat =
		{
			mDebugSphereRenderCommand.worldMatrix.Transpose(),
			mDebugSphereRenderCommand.worldMatrix.Invert() // hlsl이 col-major라 전치 생략
		};

		mpDeviceContext->UpdateSubresource(
			mpCBWorldMatrixGPU,
			0,
			nullptr,
			&cbWorldMat,
			0,
			0
		);

		mpDeviceContext->DrawIndexed(
			mDebugSphereRenderCommand.pMesh->GetIndexCount(),
			0,
			0
		);
	}

	// resolve multisample
	if (mbMultiSampling)
	{
		ID3D11Resource* pMultiSampleBuffer = nullptr;
		mpMultiSampleRenderTargetViewGPU->GetResource(&pMultiSampleBuffer);

		ID3D11Resource* pHDRBuffer = nullptr;
		mpHDRRenderTargetViewGPU->GetResource(&pHDRBuffer);

		mpDeviceContext->ResolveSubresource(
			pHDRBuffer,
			0,
			pMultiSampleBuffer,
			0,
			DXGI_FORMAT_R16G16B16A16_FLOAT
		);

		SafeRelease(pHDRBuffer);
		SafeRelease(pMultiSampleBuffer);
	}

	// HDR to SDR
	{
		mpDeviceContext->OMSetRenderTargets(1, &mpSDRRenderTargetViewGPU, nullptr);

		ShaderManager& shaderManager = ShaderManager::GetInstance();

		ID3D11VertexShader* const pVSSprite = shaderManager.GetVertexShaderOrNull(SHADER_PATH("VSFullScreen.hlsl"));
		ASSERT(pVSSprite != nullptr);

		mpDeviceContext->VSSetShader(pVSSprite, nullptr, 0);

		mpDeviceContext->RSSetState(mRasterizerStateMap[ERasterizerType::SOLID]);

		ID3D11PixelShader* const pPSSprite = shaderManager.GetPixelShaderOrNull(SHADER_PATH("PSFullScreen.hlsl"));
		ASSERT(pPSSprite != nullptr);

		mpDeviceContext->PSSetShader(pPSSprite, nullptr, 0);
		mpDeviceContext->PSSetSamplers(0, 1, &mSamplerStateMap[ESamplerType::LINEAR_WRAP]);
		mpDeviceContext->PSSetShaderResources(0, 1, &mpHDRResourceViewGPU);

		mpDeviceContext->OMSetDepthStencilState(mDepthStencilStateMap[EDepthStencilType::DEPTH_DISABLED], 0);

		MeshManager& meshManager = MeshManager::GetInstance();
		Mesh* const pSpriteMesh = meshManager.GetMeshOrNull("Square");
		ASSERT(pSpriteMesh != nullptr);

		pSpriteMesh->Bind(*mpDeviceContext);
		mpDeviceContext->DrawIndexed(pSpriteMesh->GetIndexCount(), 0, 0);

		ID3D11ShaderResourceView* const pNullSRV = nullptr;
		mpDeviceContext->PSSetShaderResources(0, 1, &pNullSRV);
	}

	// SDR to backbuffer
	ID3D11Resource* pSDRBuffer = nullptr;
	ID3D11Resource* pBackBuffer = nullptr;
	{
		mpSDRRenderTargetViewGPU->GetResource(&pSDRBuffer);
		mpBackBufferViewGPU->GetResource(&pBackBuffer);

		mpDeviceContext->CopyResource(pBackBuffer, pSDRBuffer);
	}
	SafeRelease(pBackBuffer);
	SafeRelease(pSDRBuffer);
}

void Renderer::BeginUIFrame() const
{
	mpDeviceContext->OMSetRenderTargets(1, &mpBackBufferViewGPU, nullptr);

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
}

void Renderer::EndUIFrame() const
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::OnResize(const int width, const int height)
{
	ASSERT(width > 0);
	ASSERT(height > 0);
	ASSERT(mpDevice != nullptr);
	ASSERT(mpDeviceContext != nullptr);

	mpDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	SafeRelease(mpBackBufferViewGPU);

	SafeRelease(mpMultiSampleRenderTargetViewGPU);
	SafeRelease(mpMultiSampleDepthStencilViewGPU);

	SafeRelease(mpHDRRenderTargetViewGPU);
	SafeRelease(mpHDRResourceViewGPU);

	SafeRelease(mpSDRRenderTargetViewGPU);
	SafeRelease(mpCommonDepthStencilViewGPU);

	// backbuffer
	HRESULT hr = mpSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "SwapChain - ResizeBuffers");

		ASSERT(false);

		return;
	}

	ID3D11Texture2D* pBackBuffer = nullptr;
	{
		hr = mpSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "GetBuffer");

			ASSERT(false);

			return;
		}

		hr = mpDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mpBackBufferViewGPU);

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreateRenderTargetView - BackBuffer");

			ASSERT(false);

			return;
		}
	}
	SafeRelease(pBackBuffer);

	// multisample
	{
		D3D11_TEXTURE2D_DESC multisampleBufferDesc;
		ZeroMemory(&multisampleBufferDesc, sizeof(multisampleBufferDesc));

		multisampleBufferDesc.Width = width;
		multisampleBufferDesc.Height = height;
		multisampleBufferDesc.MipLevels = 1;
		multisampleBufferDesc.ArraySize = 1;
		multisampleBufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		multisampleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		multisampleBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		UINT quality = 0;
		UINT sampleCount = 1;
		hr = mpDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &quality);

		if (SUCCEEDED(hr) && quality > 0)
		{
			sampleCount = 4;
			--quality;
		}

		multisampleBufferDesc.SampleDesc.Count = sampleCount;
		multisampleBufferDesc.SampleDesc.Quality = quality;

		ID3D11Texture2D* pMultisampleBuffer = nullptr;
		{
			hr = mpDevice->CreateTexture2D(&multisampleBufferDesc, nullptr, &pMultisampleBuffer);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateTexture2D - MultisampleBuffer");

				ASSERT(false);

				return;
			}

			hr = mpDevice->CreateRenderTargetView(pMultisampleBuffer, nullptr, &mpMultiSampleRenderTargetViewGPU);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateRenderTargetView - MultisampleView");

				ASSERT(false);

				return;
			}
		}
		SafeRelease(pMultisampleBuffer);

		D3D11_TEXTURE2D_DESC multisampleDepthStencilBufferDesc;
		ZeroMemory(&multisampleDepthStencilBufferDesc, sizeof(multisampleDepthStencilBufferDesc));

		multisampleDepthStencilBufferDesc.Width = width;
		multisampleDepthStencilBufferDesc.Height = height;
		multisampleDepthStencilBufferDesc.MipLevels = 1;
		multisampleDepthStencilBufferDesc.ArraySize = 1;
		multisampleDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		multisampleDepthStencilBufferDesc.SampleDesc.Count = sampleCount;
		multisampleDepthStencilBufferDesc.SampleDesc.Quality = quality;
		multisampleDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		multisampleDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ID3D11Texture2D* pMultisampleDepthStencilBuffer = nullptr;
		{
			hr = mpDevice->CreateTexture2D(&multisampleDepthStencilBufferDesc, nullptr, &pMultisampleDepthStencilBuffer);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateTexture2D - MultisampleDepthBuffer");

				ASSERT(false);

				return;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC multisampleDepthStencilViewDesc;
			ZeroMemory(&multisampleDepthStencilViewDesc, sizeof(multisampleDepthStencilViewDesc));

			multisampleDepthStencilViewDesc.Format = multisampleDepthStencilBufferDesc.Format;
			multisampleDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

			hr = mpDevice->CreateDepthStencilView(pMultisampleDepthStencilBuffer, &multisampleDepthStencilViewDesc, &mpMultiSampleDepthStencilViewGPU);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateDepthStencilView - MultisampleDepthStencilBuffer");

				ASSERT(false);

				return;
			}
		}
		SafeRelease(pMultisampleDepthStencilBuffer);
	}

	// non multisample buffers
	{
		D3D11_TEXTURE2D_DESC hdrBufferDesc;
		ZeroMemory(&hdrBufferDesc, sizeof(hdrBufferDesc));

		hdrBufferDesc.Width = width;
		hdrBufferDesc.Height = height;
		hdrBufferDesc.MipLevels = 1;
		hdrBufferDesc.ArraySize = 1;
		hdrBufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		hdrBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		hdrBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		hdrBufferDesc.SampleDesc.Count = 1;
		hdrBufferDesc.SampleDesc.Quality = 0;

		ID3D11Texture2D* pHDRBuffer = nullptr;
		{
			hr = mpDevice->CreateTexture2D(&hdrBufferDesc, nullptr, &pHDRBuffer);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateTexture2D - HDRBuffer");

				ASSERT(false);

				return;
			}

			hr = mpDevice->CreateRenderTargetView(pHDRBuffer, nullptr, &mpHDRRenderTargetViewGPU);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateRenderTargetView - HDRBufferView");

				ASSERT(false);

				return;
			}

			hr = mpDevice->CreateShaderResourceView(pHDRBuffer, nullptr, &mpHDRResourceViewGPU);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateShaderResourceView");

				ASSERT(false);

				return;
			}
		}
		SafeRelease(pHDRBuffer);

		D3D11_TEXTURE2D_DESC sdrBufferDesc;
		ZeroMemory(&sdrBufferDesc, sizeof(sdrBufferDesc));

		sdrBufferDesc.Width = width;
		sdrBufferDesc.Height = height;
		sdrBufferDesc.MipLevels = 1;
		sdrBufferDesc.ArraySize = 1;
		sdrBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sdrBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		sdrBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		sdrBufferDesc.SampleDesc.Count = 1;
		sdrBufferDesc.SampleDesc.Quality = 0;

		ID3D11Texture2D* pSDRBuffer = nullptr;
		{
			hr = mpDevice->CreateTexture2D(&sdrBufferDesc, nullptr, &pSDRBuffer);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateTexture2D - SDRBuffer");

				ASSERT(false);

				return;
			}

			hr = mpDevice->CreateRenderTargetView(pSDRBuffer, nullptr, &mpSDRRenderTargetViewGPU);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateRenderTargetView - SDRView");

				ASSERT(false);

				return;
			}
		}
		SafeRelease(pSDRBuffer);


		D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
		ZeroMemory(&depthStencilBufferDesc, sizeof(depthStencilBufferDesc));

		depthStencilBufferDesc.Width = width;
		depthStencilBufferDesc.Height = height;
		depthStencilBufferDesc.MipLevels = 1;
		depthStencilBufferDesc.ArraySize = 1;
		depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
		depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ID3D11Texture2D* pDepthStencilBuffer = nullptr;
		{
			hr = mpDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &pDepthStencilBuffer);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateTexture2D - CommonDepthStencilBuffer");

				ASSERT(false);

				return;
			}

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

			depthStencilViewDesc.Format = depthStencilBufferDesc.Format;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			hr = mpDevice->CreateDepthStencilView(pDepthStencilBuffer, &depthStencilViewDesc, &mpCommonDepthStencilViewGPU);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "CreateDepthStencilView");

				ASSERT(false);

				return;
			}
		}
		SafeRelease(pDepthStencilBuffer);
	}

	mViewport.TopLeftX = 0.f;
	mViewport.TopLeftY = 0.f;
	mViewport.Width = static_cast<FLOAT>(width);
	mViewport.Height = static_cast<FLOAT>(height);
	mViewport.MinDepth = 0.f;
	mViewport.MaxDepth = 1.f;
}

void Renderer::UpdateCBFrame(const Vector3& cameraPos, const Matrix& viewProj)
{
	const CBFrame cbFrame =
	{
		cameraPos,
		0.f,
		viewProj.Transpose()
	};

	mpDeviceContext->UpdateSubresource(
		mpCBFrameGPU,
		0,
		nullptr,
		&cbFrame,
		0,
		0
	);
}

void Renderer::AddMeshComponentList(const std::string& sceneName)
{
	ASSERT(mSceneComponents.find(sceneName) == mSceneComponents.end());

	std::vector<MeshComponent*> meshComponentList;
	meshComponentList.reserve(DEFAULT_BUFFER_SIZE);

	mSceneComponents[sceneName] = std::move(meshComponentList);
}

void Renderer::RemoveMeshComponentList(const std::string& sceneName)
{
	ASSERT(mSceneComponents.find(sceneName) != mSceneComponents.end());

	mSceneComponents.erase(sceneName);
}

void Renderer::AddMeshComponent(const std::string& sceneName, MeshComponent* const pMeshComponent)
{
	ASSERT(mSceneComponents.find(sceneName) != mSceneComponents.end());

	mSceneComponents[sceneName].push_back(pMeshComponent);
}

void Renderer::RemoveMeshComponent(const std::string& sceneName, MeshComponent* const pMeshComponent)
{
	ASSERT(mSceneComponents.find(sceneName) != mSceneComponents.end());

	std::vector<MeshComponent*>& meshComponentList = mSceneComponents[sceneName];

#define VECTOR_ITER std::vector<MeshComponent*>::iterator

	for (VECTOR_ITER iter = meshComponentList.begin(); iter != meshComponentList.end(); ++iter)
	{
		if (*iter == pMeshComponent)
		{
			meshComponentList.erase(iter);
			break;
		}
	}

#undef VECTOR_ITER
}

bool Renderer::TryInitialize(const HWND hWnd)
{
	ASSERT(hWnd != nullptr);
	ASSERT(spInstance == nullptr);

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;

	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&pDevice,
		nullptr,
		&pDeviceContext
	);

	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "D3D11CreateDevice");

		ASSERT(false);

		return false;
	}

	// 주사율 조사 및 스왑체인 생성
	IDXGIDevice* pDXGIDevice = nullptr;
	IDXGIAdapter* pAdapter = nullptr;
	IDXGIOutput* pOutput = nullptr;

	IDXGISwapChain1* pSwapChain = nullptr;
	UINT refreshRate = 60;
	{
		hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDXGIDevice));

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "QueryInterface - IDXGIDevice");

			ASSERT(false);

			return false;
		}

		hr = pDXGIDevice->GetAdapter(&pAdapter);

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "GetAdapter - IDXGIAdapter");

			ASSERT(false);

			return false;
		}

		// 첫 번째 모니터 획득
		hr = pAdapter->EnumOutputs(0, &pOutput);

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "EnumOutput");

			ASSERT(false);

			return false;
		}

		DXGI_OUTPUT_DESC outputDesc;
		hr = pOutput->GetDesc(&outputDesc);

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "GetDesc");

			ASSERT(false);

			return false;
		}

		DEVMODEW dm;
		ZeroMemory(&dm, sizeof(dm));

		dm.dmSize = sizeof(dm);
		if (!EnumDisplaySettings(outputDesc.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
		{
			ASSERT(false);

			return false;
		}

		refreshRate = dm.dmDisplayFrequency;

		IDXGIFactory* pBaseFactory = nullptr;
		hr = pAdapter->GetParent(IID_PPV_ARGS(&pBaseFactory));

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "GetParent - IDXGIFactory");

			ASSERT(false);

			return false;
		}

		IDXGIFactory2* pDXGIFactory = nullptr;
		hr = pBaseFactory->QueryInterface(IID_PPV_ARGS(&pDXGIFactory));

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "QueryInterface - IDXGIFactory2");

			ASSERT(false);

			return false;
		}

		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));

		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Stereo = false;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferCount = 2;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfd;
		ZeroMemory(&scfd, sizeof(scfd));

		scfd.Windowed = true;

		hr = pDXGIFactory->CreateSwapChainForHwnd(
			pDevice,
			hWnd,
			&sd,
			&scfd,
			nullptr,
			&pSwapChain
		);

		if (FAILED(hr))
		{
			LOG_SYSTEM_ERROR(hr, "CreateSwapChainForHwnd");

			ASSERT(false);

			return false;
		}

		SafeRelease(pDXGIFactory);
		SafeRelease(pBaseFactory);
	}
	SafeRelease(pOutput);
	SafeRelease(pAdapter);
	SafeRelease(pDXGIDevice);

	TextureManager::Initialize(*pDevice);
	ShaderManager::Initialize(*pDevice);
	MeshManager::Initialize(*pDevice);
	MaterialManager::Initialize(*pDevice);
	ModelManager::Initialize();

	spInstance = new Renderer(
		pDevice,
		pDeviceContext,
		pSwapChain,
		refreshRate
	);

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
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.f, nullptr, io.Fonts->GetGlyphRangesKorean());

	if (!ImGui_ImplWin32_Init(hWnd))
	{
		LOG_SYSTEM_ERROR(E_FAIL, "ImGui - Win32_Init");

		ASSERT(false);
	}

	if (!ImGui_ImplDX11_Init(pDevice, pDeviceContext))
	{
		LOG_SYSTEM_ERROR(E_FAIL, "ImGui - DX11_Init");

		ASSERT(false);
	}

	return true;
}

void Renderer::DrawEditorUI()
{
	ImGui::PushID("Renderer");

	ImGui::Checkbox(UTF8_TEXT("수직동기화"), &mbVSync);

	ImGui::Checkbox(UTF8_TEXT("멀티샘플링"), &mbMultiSampling);

	ImGui::Checkbox(UTF8_TEXT("절두체 컬링"), &mbViewFrustumCulling);

	ImGui::Checkbox(UTF8_TEXT("와이어프레임(F4)"), &mbWireframeMode);

	ImGui::SliderFloat4(UTF8_TEXT("화면 초기화 색상"), mClearColor, 0.f, 1.f);

	ImGui::PopID();
}

Vector3 Renderer::Unproject(const Vector3 v) const
{
	const Matrix& viewProj = mpMainCameraComponent->GetViewProjMatrix();

	const Matrix invViewProj = viewProj.Invert();

	return Vector3::Transform(v, invViewProj);
}
