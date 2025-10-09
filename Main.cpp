#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#include <cassert>
#include <cctype>
#include <algorithm>

// winapi
#include <Windows.h>

// d3d
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>

#include "SimpleMath.h"

#include "DebugHelper.h"
#include "ComHelper.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <wrl.h>

const TCHAR* const CLASS_NAME = TEXT("GyulEngine");

static HINSTANCE shInstance;
static HWND shWnd;

static int sWidth;
static int sHeight;

static ID3D11Device* spDevice;
static ID3D11DeviceContext* spDeviceContext;

static IDXGISwapChain* spSwapChain;
static ID3D11RenderTargetView* spRenderTargetViewGPU;
static ID3D11Texture2D* spDepthStencilBuffer;
static ID3D11DepthStencilView* spDepthStencilViewGPU;

static ID3D11VertexShader* spVertexShader;
static ID3DBlob* spVertexShaderBlob;

static ID3D11PixelShader* spPixelShader;

using namespace DirectX;
using namespace DirectX::SimpleMath;

struct Vertex
{
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;
};
static_assert(sizeof(Vertex) % 4 == 0);

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<int32_t> indices;

	ID3D11Buffer* vertexBufferGPU;
	ID3D11Buffer* indexBufferGPU;
	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology;
};

struct CBWorldMatrix
{
	Matrix world;
};
static_assert(sizeof(CBWorldMatrix) % 4 == 0);

struct CBMainCamera
{
	Vector3 cameraPos;
	float dummy;
	Matrix viewProj;
};
static_assert(sizeof(CBMainCamera) % 16 == 0);

struct Actor
{
	Vector3 pos;
	Vector3 scale;
	Vector3 rotationAxis;
	Vector3 frontDir;

	Mesh mesh;
};

static ID3D11InputLayout* spInputLayout;

ID3D11Buffer* spVertexBufferGPU;
ID3D11Buffer* spIndexBufferGPU;

D3D11_VIEWPORT sViewport;

static Actor sActor;
static CBWorldMatrix sCBWorldMatrix;
static ID3D11Buffer* spCBWorldMatrixGPU;

static CBMainCamera sCBMainCamera;
static ID3D11Buffer* spCBMainCameraGPU;
static float sFOV;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void OnResize(const int width, const int height);

ID3D11Buffer* CreateConstantBufferOrNull(void* pData, const UINT byteSize);

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     PWSTR     pCmdLine,
	_In_     int       nShowCmd
)
{
	// initialize
	{
		// win32
		shInstance = hInstance;

		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(wc));

		wc.cbSize = sizeof(wc);
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.lpszClassName = CLASS_NAME;

		int errorCode = 0;
		if (!RegisterClassEx(&wc))
		{
			errorCode = GetLastError();
			LogSystemError(errorCode, "RegisterClassEx");

			ASSERT(false);

			return errorCode;
		}

		shWnd = CreateWindowEx(
			0,
			CLASS_NAME,
			CLASS_NAME,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);

		if (shWnd == nullptr)
		{
			errorCode = GetLastError();
			LogSystemError(errorCode, "CreateWindowEx");

			ASSERT(false);

			return errorCode;
		}

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
		sd.BufferDesc.RefreshRate.Numerator = 120;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = shWnd;
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
			&spSwapChain,
			&spDevice,
			nullptr,
			&spDeviceContext
		);

		if (FAILED(hr))
		{
			LogSystemError(hr, "CreateDeviceAndSwapChain");

			ASSERT(false);

			return hr;
		}

		// shader
		UINT shaderCompileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		shaderCompileFlags |= D3DCOMPILE_DEBUG;
		shaderCompileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ID3DBlob* pErrorBlob = nullptr;

		hr = D3DCompileFromFile(
			TEXT("VSBasic.hlsl"),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"vs_5_0",
			shaderCompileFlags,
			0,
			&spVertexShaderBlob,
			&pErrorBlob
		);

		if (FAILED(hr))
		{
			LogSystemError(hr, "D3DCompileFromFile - VSShader");

			if (pErrorBlob != nullptr)
			{
				LogSystemError(hr, reinterpret_cast<char*>(pErrorBlob->GetBufferPointer()));
			}

			ASSERT(false);

			return hr;
		}

		SafeRelease(pErrorBlob);

		hr = spDevice->CreateVertexShader(
			spVertexShaderBlob->GetBufferPointer(),
			spVertexShaderBlob->GetBufferSize(),
			nullptr,
			&spVertexShader
		);

		if (FAILED(hr))
		{
			LogSystemError(hr, "CreateVertexShader - VSShader");

			if (pErrorBlob != nullptr)
			{
				LogSystemError(hr, reinterpret_cast<char*>(pErrorBlob->GetBufferPointer()));
			}

			ASSERT(false);

			return hr;
		}

		ID3DBlob* pShaderBlob = nullptr;

		hr = D3DCompileFromFile(
			TEXT("PSBasic.hlsl"),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"ps_5_0",
			0,
			0,
			&pShaderBlob,
			&pErrorBlob
		);

		if (FAILED(hr))
		{
			LogSystemError(hr, "D3DCompileFromFile - PSShader");

			if (pErrorBlob != nullptr)
			{
				LogSystemError(hr, reinterpret_cast<char*>(pErrorBlob->GetBufferPointer()));
			}

			ASSERT(false);

			return hr;
		}

		SafeRelease(pErrorBlob);

		hr = spDevice->CreatePixelShader(
			pShaderBlob->GetBufferPointer(),
			pShaderBlob->GetBufferSize(),
			nullptr,
			&spPixelShader
		);

		if (FAILED(hr))
		{
			LogSystemError(hr, "CreatePixelShader - PSShader");

			ASSERT(false);

			return hr;
		}

		SafeRelease(pShaderBlob);

		// ui
		IMGUI_CHECKVERSION();
		ImGuiContext* pImGuiContext = ImGui::CreateContext();
		if (pImGuiContext == nullptr)
		{
			LogSystemError(E_FAIL, "ImGui - CreateContext");

			ASSERT(false);
		}

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		if (!ImGui_ImplWin32_Init(shWnd))
		{
			LogSystemError(E_FAIL, "ImGui - Win32_Init");

			ASSERT(false);
		}

		if (!ImGui_ImplDX11_Init(spDevice, spDeviceContext))
		{
			LogSystemError(E_FAIL, "ImGui - DX11_Init");

			ASSERT(false);
		}

		// data
		D3D11_INPUT_ELEMENT_DESC vertexLayoutDescs[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, uv), D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		hr = spDevice->CreateInputLayout(
			vertexLayoutDescs,
			ARRAYSIZE(vertexLayoutDescs),
			spVertexShaderBlob->GetBufferPointer(),
			spVertexShaderBlob->GetBufferSize(),
			&spInputLayout
		);

		if (FAILED(hr))
		{
			LogSystemError(hr, "CreateInputLayout");

			ASSERT(false);

			return hr;
		}

		sActor.pos = { 0.f, 0.f, 0.f };
		sActor.scale = { 1.f, 1.f, 1.f };
		sActor.rotationAxis = { 0.f, 0.f, 0.f };

		sActor.mesh.vertices = {
			{ { 0.f, 0.5f, 0.f }, {},{} },
			{ { 0.5f, -0.5f, 0.f }, {}, {} },
			{ { -0.5f, -0.5f, 0.f }, {}, {} }
		};

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.ByteWidth = static_cast<UINT>(sActor.mesh.vertices.size()) * sizeof(Vertex);
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.StructureByteStride = sizeof(Vertex);

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));

		vertexBufferData.pSysMem = sActor.mesh.vertices.data();

		hr = spDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &spVertexBufferGPU);

		sActor.mesh.vertexBufferGPU = spVertexBufferGPU;

		if (FAILED(hr))
		{
			LogSystemError(hr, "CreateBuffer - VertexBuffer");

			ASSERT(false);

			return hr;
		}

		sActor.mesh.indices = {
			0, 1, 2
		};

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.ByteWidth = static_cast<UINT>(sActor.mesh.indices.size()) * sizeof(int32_t);
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.StructureByteStride = sizeof(int32_t);

		D3D11_SUBRESOURCE_DATA indexBufferData;
		ZeroMemory(&indexBufferData, sizeof(indexBufferData));

		indexBufferData.pSysMem = sActor.mesh.indices.data();

		hr = spDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &spIndexBufferGPU);

		if (FAILED(hr))
		{
			LogSystemError(hr, "CreateBuffer - IndexBuffer");

			ASSERT(false);

			return hr;
		}

		sActor.mesh.indexBufferGPU = spIndexBufferGPU;
		sActor.mesh.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		spCBWorldMatrixGPU = CreateConstantBufferOrNull(&sCBWorldMatrix, sizeof(CBWorldMatrix));
		spCBMainCameraGPU = CreateConstantBufferOrNull(&sCBMainCamera, sizeof(CBMainCamera));

		sCBMainCamera.cameraPos = { 0.f, 0.f, -0.5f };
		sFOV = DirectX::XMConvertToRadians(105.f);
	}

	ShowWindow(shWnd, nShowCmd);
	UpdateWindow(shWnd);

	// run
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER prev;
	QueryPerformanceCounter(&prev);

	LARGE_INTEGER curr;

	MSG msg;
	while (true)
	{
		// processInput
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// update
			{
				ASSERT(spDeviceContext != nullptr);

				constexpr float FRAME_TIME = 1.f / 120.f;
				constexpr float MAX_DELTA_TIME = 0.05f;

				float deltaTime;
				while (true)
				{
					QueryPerformanceCounter(&curr);

					const LONGLONG delta = curr.QuadPart - prev.QuadPart;
					deltaTime = delta / static_cast<float>(freq.QuadPart);

					if (deltaTime >= FRAME_TIME)
					{
						if (deltaTime > MAX_DELTA_TIME)
						{
							deltaTime = MAX_DELTA_TIME;
						}

						break;
					}
				}

				prev = curr;

				// actor
				const Matrix translation = Matrix::CreateTranslation(sActor.pos);
				const Matrix scale = Matrix::CreateScale(sActor.scale);

				const Vector3 radians = sActor.rotationAxis * (DirectX::XM_PI / 180.f);

				const Matrix rotationX = Matrix::CreateRotationX(radians.x);
				const Matrix rotationY = Matrix::CreateRotationY(radians.y);
				const Matrix rotationZ = Matrix::CreateRotationZ(radians.z);

				sCBWorldMatrix.world = scale * rotationX * rotationY * rotationZ * translation;
				sCBWorldMatrix.world = sCBWorldMatrix.world.Transpose();

				spDeviceContext->UpdateSubresource(spCBWorldMatrixGPU, 0, nullptr, &sCBWorldMatrix, 0, 0);

				// camera
				const Matrix view = XMMatrixLookAtLH(sCBMainCamera.cameraPos, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f });

				const float aspectRatio = sWidth / static_cast<float>(sHeight);
				const Matrix proj = XMMatrixPerspectiveFovLH(sFOV, aspectRatio, 0.01f, 100.f);

				sCBMainCamera.viewProj = view * proj;
				sCBMainCamera.viewProj = sCBMainCamera.viewProj.Transpose();

				spDeviceContext->UpdateSubresource(spCBMainCameraGPU, 0, nullptr, &sCBMainCamera, 0, 0);
			}

			// generateOutput
			{
				ASSERT(spDeviceContext != nullptr);
				ASSERT(spSwapChain != nullptr);
				ASSERT(spRenderTargetViewGPU != nullptr);

				// set and clear rendertarget
				spDeviceContext->OMSetRenderTargets(1, &spRenderTargetViewGPU, nullptr);

				constexpr float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
				spDeviceContext->ClearRenderTargetView(spRenderTargetViewGPU, clearColor);

				spDeviceContext->ClearDepthStencilView(spDepthStencilViewGPU, D3D11_CLEAR_DEPTH, 1.f, 255);

				// ia
				UINT stride = sizeof(Vertex);
				UINT offset = 0;

				spDeviceContext->IASetInputLayout(spInputLayout);
				spDeviceContext->IASetPrimitiveTopology(sActor.mesh.primitiveTopology);
				spDeviceContext->IASetVertexBuffers(0, 1, &sActor.mesh.vertexBufferGPU, &stride, &offset);
				spDeviceContext->IASetIndexBuffer(sActor.mesh.indexBufferGPU, DXGI_FORMAT_R32_UINT, 0);

				// vs
				ID3D11Buffer* constantBuffers[] = {
					spCBWorldMatrixGPU, spCBMainCameraGPU
				};

				spDeviceContext->VSSetShader(spVertexShader, nullptr, 0);
				spDeviceContext->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

				// rs
				spDeviceContext->RSSetViewports(1, &sViewport);

				// ps
				spDeviceContext->PSSetShader(spPixelShader, nullptr, 0);
				spDeviceContext->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

				// draw
				spDeviceContext->DrawIndexed(static_cast<UINT>(sActor.mesh.indices.size()), 0, 0);
			}

			// ui
			ImGui_ImplWin32_NewFrame();
			ImGui_ImplDX11_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Main");
			{
				ImGuiIO& io = ImGui::GetIO();
				ImGui::Text("FPS: %d", static_cast<int>(io.Framerate));

				if (ImGui::BeginChild("Actor", { 0, 0 }, true))
				{
					ImGui::Text("Actor");
					ImGui::Separator();

					if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen))
					{
						if (ImGui::BeginTable("XYZ", 3, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV))
						{
							ImGui::TableSetupColumn("X");
							ImGui::TableSetupColumn("Y");
							ImGui::TableSetupColumn("Z");
							ImGui::TableHeadersRow();

							ImGui::TableNextRow();
							constexpr const char* const labels[] = {
								"##PosX", "##PosY", "##PosZ"
							};

							float* pBindingData = reinterpret_cast<float*>(&sActor.pos);
							for (int i = 0; i < 3; ++i)
							{
								ImGui::TableSetColumnIndex(i);
								ImGui::SetNextItemWidth(-FLT_MIN);

								ImGui::DragFloat(labels[i], pBindingData + i, 0.1f, sWidth * -0.5f, sWidth * 0.5f, "%.1f");
							}
						}
						ImGui::EndTable();
					}

					if (ImGui::CollapsingHeader("Scale", ImGuiTreeNodeFlags_DefaultOpen))
					{
						if (ImGui::BeginTable("XYZ", 3, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV))
						{
							ImGui::TableSetupColumn("X");
							ImGui::TableSetupColumn("Y");
							ImGui::TableSetupColumn("Z");
							ImGui::TableHeadersRow();

							ImGui::TableNextRow();
							constexpr const char* const labels[] = {
								"##ScaleX", "##ScaleY", "##ScaleZ"
							};

							constexpr float MIN_SCALE = 1.f;
							constexpr float MAX_SCALE = 100.f;

							float* pBindingData = reinterpret_cast<float*>(&sActor.scale);
							for (int i = 0; i < 3; ++i)
							{
								ImGui::TableSetColumnIndex(i);
								ImGui::SetNextItemWidth(-FLT_MIN);

								ImGui::InputFloat(labels[i], pBindingData + i, 0.f, 0.f, "%.1f");

								pBindingData[i] = std::clamp(pBindingData[i], MIN_SCALE, MAX_SCALE);
							}

							ImGui::TableNextRow();
							constexpr const char* const sliderLabels[] = {
								"##ScaleXSlider", "##ScaleYSlider", "##ScaleZSlider"
							};

							for (int i = 0; i < 3; ++i)
							{
								ImGui::TableSetColumnIndex(i);
								ImGui::SetNextItemWidth(-FLT_MIN);

								ImGui::SliderFloat(sliderLabels[i], pBindingData + i, MIN_SCALE, MAX_SCALE, "%.1f");
							}
						}
						ImGui::EndTable();
					}

					if (ImGui::CollapsingHeader("Rotation", ImGuiTreeNodeFlags_DefaultOpen))
					{
						if (ImGui::BeginTable("XYZ", 3, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerV))
						{
							ImGui::TableSetupColumn("X");
							ImGui::TableSetupColumn("Y");
							ImGui::TableSetupColumn("Z");
							ImGui::TableHeadersRow();

							ImGui::TableNextRow();
							constexpr const char* const labels[] = {
								"##RotationX", "##RotationY", "##RotationZ"
							};

							constexpr float MAX_DEGREE = 360.f;

							float* pBindingData = reinterpret_cast<float*>(&sActor.rotationAxis);
							for (int i = 0; i < 3; ++i)
							{
								ImGui::TableSetColumnIndex(i);
								ImGui::SetNextItemWidth(-FLT_MIN);

								ImGui::InputFloat(labels[i], pBindingData + i, 0.f, 0.f, "%.1f");

								// wrapping
								if (abs(pBindingData[i]) > MAX_DEGREE)
								{
									pBindingData[i] -= MAX_DEGREE * static_cast<int>(pBindingData[i] / MAX_DEGREE);
								}
							}

							ImGui::TableNextRow();
							constexpr const char* const sliderLabels[] = {
								"##RotationXSlider", "##RotationYSlider", "##RotationZSlider"
							};

							for (int i = 0; i < 3; ++i)
							{
								ImGui::TableSetColumnIndex(i);
								ImGui::SetNextItemWidth(-FLT_MIN);

								ImGui::DragFloat(sliderLabels[i], pBindingData + i, 1.f, -MAX_DEGREE, MAX_DEGREE, "%.1f");
							}
						}
						ImGui::EndTable();
					}
				}
				ImGui::EndChild();
			}
			ImGui::End();

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			// present
			spSwapChain->Present(0, 0);
		}
	}

	// destroy
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		SafeRelease(spCBMainCameraGPU);
		SafeRelease(spCBWorldMatrixGPU);
		SafeRelease(spIndexBufferGPU);
		SafeRelease(spVertexBufferGPU);
		SafeRelease(spInputLayout);

		SafeRelease(spPixelShader);
		SafeRelease(spVertexShader);
		SafeRelease(spVertexShaderBlob);

		SafeRelease(spDepthStencilViewGPU);
		SafeRelease(spDepthStencilBuffer);
		SafeRelease(spRenderTargetViewGPU);
		SafeRelease(spSwapChain);

		SafeRelease(spDeviceContext);
		SafeRelease(spDevice);

		UnregisterClass(CLASS_NAME, shInstance);
	}

	return (int)msg.wParam;
}

LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		{
			ASSERT(spSwapChain != nullptr);

			const int width = LOWORD(lParam);
			const int height = HIWORD(lParam);

			OnResize(width, height);
		}
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

void OnResize(const int width, const int height)
{
	ASSERT(width > 0);
	ASSERT(height > 0);
	ASSERT(spDeviceContext != nullptr);

	if (spSwapChain == nullptr)
	{
		return;
	}

	sWidth = width;
	sHeight = height;

	spDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	SafeRelease(spRenderTargetViewGPU);

	HRESULT hr = spSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
	{
		LogSystemError(hr, "SwapChain - ResizeBuffers");

		ASSERT(false);

		return;
	}

	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = spSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));

	if (FAILED(hr))
	{
		LogSystemError(hr, "SwapChain - GetBuffer");

		ASSERT(false);

		return;
	}

	hr = spDevice->CreateRenderTargetView(pBackBuffer, nullptr, &spRenderTargetViewGPU);
	SafeRelease(pBackBuffer);

	if (FAILED(hr))
	{
		LogSystemError(hr, "Device - CreateRenderTargetView");

		ASSERT(false);

		return;
	}

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

	hr = spDevice->CreateTexture2D(&depthBufferDesc, nullptr, &spDepthStencilBuffer);

	if (FAILED(hr))
	{
		LogSystemError(hr, "CreateTexture2D - DepthStencil");

		ASSERT(false);

		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthBufferDesc.Format = depthBufferDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = spDevice->CreateDepthStencilView(spDepthStencilBuffer, &depthStencilViewDesc, &spDepthStencilViewGPU);

	if (FAILED(hr))
	{
		LogSystemError(hr, "CreateDepthStencilView");

		ASSERT(false);

		return;
	}

	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();

	sViewport.TopLeftX = 0.f;
	sViewport.TopLeftY = 0.f;
	sViewport.Width = static_cast<FLOAT>(width);
	sViewport.Height = static_cast<FLOAT>(height);
}

ID3D11Buffer* CreateConstantBufferOrNull(void* pData, const UINT byteSize)
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.ByteWidth = byteSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(bufferData));

	bufferData.pSysMem = pData;

	ID3D11Buffer* pRet = nullptr;

	const HRESULT hr = spDevice->CreateBuffer(&bufferDesc, &bufferData, &pRet);

	if (FAILED(hr))
	{
		LogSystemError(hr, "CreateBuffer - ConstantBuffer");

		ASSERT(false);
	}

	return pRet;
}
