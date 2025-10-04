#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#include <cassert>
#include <cctype>

// winapi
#include <Windows.h>

// d3d
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>

#include "SimpleMath.h"

#include "DebugHelper.h"
#include "ComHelper.h"

const TCHAR* const CLASS_NAME = TEXT("GyulEngine");

enum
{
	DEFAULT_WIDTH = 1280,
	DEFAULT_HEIGHT = 720
};

static HINSTANCE shInstance;
static HWND shWnd;

static ID3D11Device* spDevice;
static ID3D11DeviceContext* spDeviceContext;
static IDXGISwapChain* spSwapChain;
static ID3D11RenderTargetView* spRenderTargetView;

static ID3D11VertexShader* spVertexShader;
static ID3DBlob* spVertexShaderBlob;

static ID3D11PixelShader* spPixelShader;

using namespace DirectX::SimpleMath;

struct Vertex
{
	Vector3 pos;
};
static_assert(sizeof(Vertex) % 4 == 0);

static ID3D11InputLayout* spInputLayout;

static ID3D11Buffer* spVertexBuffer;
static ID3D11Buffer* spIndexBuffer;

D3D11_VIEWPORT sViewport;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

		RECT clientRect;
		clientRect.left = 0;
		clientRect.top = 0;
		clientRect.right = DEFAULT_WIDTH;
		clientRect.bottom = DEFAULT_HEIGHT;

		AdjustWindowRectEx(&clientRect, WS_OVERLAPPEDWINDOW, false, 0);

		shWnd = CreateWindowEx(
			0,
			CLASS_NAME,
			CLASS_NAME,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			clientRect.right, clientRect.bottom,
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

		ID3D11Texture2D* pBackBuffer = nullptr;
		hr = spSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
		if (FAILED(hr))
		{
			LogSystemError(hr, "SwapChain - GetBuffer");

			ASSERT(false);

			return hr;
		}

		spDevice->CreateRenderTargetView(pBackBuffer, nullptr, &spRenderTargetView);
		SafeRelease(pBackBuffer);

		sViewport.TopLeftX = 0.f;
		sViewport.TopLeftY = 0.f;
		sViewport.Width = static_cast<FLOAT>(DEFAULT_WIDTH);
		sViewport.Height = static_cast<FLOAT>(DEFAULT_HEIGHT);

		// shader
		ID3DBlob* pErrorBlob = nullptr;

		hr = D3DCompileFromFile(
			TEXT("VSBasic.hlsl"),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"vs_5_0",
			0,
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

		// data
		D3D11_INPUT_ELEMENT_DESC vertexLayoutDescs[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

		const Vector3 vertices[] = {
			{ 0.f, 0.5f, 0.f },
			{ 0.5f, -0.5f, 0.f },
			{ -0.5f, -0.5f, 0.f }
		};

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.ByteWidth = sizeof(vertices);
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = sizeof(Vertex);

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		vertexBufferData.pSysMem = vertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		hr = spDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &spVertexBuffer);

		if (FAILED(hr))
		{
			LogSystemError(hr, "CreateBuffer - VertexBuffer");

			ASSERT(false);

			return hr;
		}

		const int32_t indices[] = {
			0, 1, 2
		};

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.ByteWidth = sizeof(indices);
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = sizeof(int32_t);

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = indices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		hr = spDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &spIndexBuffer);

		if (FAILED(hr))
		{
			LogSystemError(hr, "CreateBuffer - IndexBuffer");

			ASSERT(false);

			return hr;
		}
	}

	// run
	ShowWindow(shWnd, nShowCmd);
	UpdateWindow(shWnd);

	MSG msg;
	while (true)
	{
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
			ASSERT(spDeviceContext != nullptr);

			// generateOutput
			ASSERT(spDeviceContext != nullptr);
			ASSERT(spSwapChain != nullptr);
			ASSERT(spRenderTargetView != nullptr);

			constexpr float clearColor[] = { 1.f, 1.f, 0.f, 1.f };
			spDeviceContext->ClearRenderTargetView(spRenderTargetView, clearColor);

			UINT stride = sizeof(Vertex);
			UINT offset = 0;

			// om
			spDeviceContext->IASetInputLayout(spInputLayout);
			spDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			spDeviceContext->IASetVertexBuffers(0, 1, &spVertexBuffer, &stride, &offset);
			spDeviceContext->IASetIndexBuffer(spIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			// vs
			spDeviceContext->VSSetShader(spVertexShader, nullptr, 0);

			// rs
			spDeviceContext->RSSetViewports(1, &sViewport);

			// ps
			spDeviceContext->PSSetShader(spPixelShader, nullptr, 0);

			// om
			spDeviceContext->OMSetRenderTargets(1, &spRenderTargetView, nullptr);

			// draw
			spDeviceContext->DrawIndexed(3, 0, 0);

			spSwapChain->Present(0, 0);
		}
	}

	// destroy
	{
		SafeRelease(spIndexBuffer);
		SafeRelease(spVertexBuffer);
		SafeRelease(spInputLayout);
		SafeRelease(spPixelShader);
		SafeRelease(spVertexShader);
		SafeRelease(spVertexShaderBlob);
		SafeRelease(spRenderTargetView);
		SafeRelease(spSwapChain);
		SafeRelease(spDeviceContext);
		SafeRelease(spDevice);
		UnregisterClass(CLASS_NAME, shInstance);
	}

	return (int)msg.wParam;
}

LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}
