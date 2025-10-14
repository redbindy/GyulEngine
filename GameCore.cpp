#include "GameCore.h"

#include "Renderer/Renderer.h"
#include "Actor.h"
#include "Component/MeshComponent.h"
#include "Component/CameraComponent.h"
#include "Component/CameraControllerComponent.h"

GameCore* GameCore::spInstance = nullptr;

static const TCHAR* const CLASS_NAME = TEXT("GyulEngine");

GameCore::GameCore(const HINSTANCE hInstance)
	: mhInstance(hInstance)
	, mhWnd(nullptr)
	, mErrorCode(S_OK)
	, mbKeyPressed{ false, }
	, mpActor(nullptr)
{
	ASSERT(hInstance != nullptr);

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
		LOG_SYSTEM_ERROR(errorCode, "RegisterClassEx");

		ASSERT(false);

		mErrorCode = HRESULT_FROM_WIN32(errorCode);

		return;
	}

	mhWnd = CreateWindowEx(
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

	if (mhWnd == nullptr)
	{
		errorCode = GetLastError();
		LOG_SYSTEM_ERROR(errorCode, "CreateWindowEx");

		MessageBox(nullptr, TEXT("Failed to create window"), TEXT("Error"), MB_OK | MB_ICONERROR);

		ASSERT(false);

		mErrorCode = HRESULT_FROM_WIN32(errorCode);

		return;
	}

	if (!Renderer::TryInitialize(mhWnd))
	{
		MessageBox(nullptr, TEXT("Failed to initialize Renderer"), TEXT("Error"), MB_OK | MB_ICONERROR);

		ASSERT(false);
	}
}

GameCore::~GameCore()
{
	delete mpCameraActor;
	delete mpActor;

	Renderer::Destroy();

	UnregisterClass(CLASS_NAME, mhInstance);
}

int GameCore::Run()
{
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
			Renderer* pRenderer = Renderer::GetInstance();
			ASSERT(pRenderer != nullptr);

			const UINT targetFrameRate = pRenderer->GetTargetFrameRate();
			const float frameTime = 1.f / static_cast<float>(targetFrameRate);

			float deltaTime;
			while (true)
			{
				QueryPerformanceCounter(&curr);

				constexpr float MAX_DELTA_TIME = 0.05f;

				const LONGLONG delta = curr.QuadPart - prev.QuadPart;
				deltaTime = delta / static_cast<float>(freq.QuadPart);

				if (deltaTime >= frameTime)
				{
					if (deltaTime > MAX_DELTA_TIME)
					{
						deltaTime = MAX_DELTA_TIME;
					}

					break;
				}

				Sleep(0);
			}

			prev = curr;

			mpCameraActor->Update(deltaTime);
			mpActor->Update(deltaTime);

			// render
			pRenderer->BeginFrame();
			{
				pRenderer->RenderScene();

				pRenderer->BeginUI();
				{
					ImGui::Begin("MainPanel");
					{
						DrawUI();
						pRenderer->DrawUI();
						mpCameraActor->DrawUI();
						mpActor->DrawUI();
					}
					ImGui::End();
				}
				pRenderer->EndUI();
			}
			pRenderer->EndFrame();
		}
	}

	return (int)msg.wParam;
}

void GameCore::DrawUI()
{
	ImGui::PushID("GameCore");
	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				PostQuitMessage(0);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
	ImGui::PopID();
}

bool GameCore::TryInitialize(const HINSTANCE hInstance, const int nShowCmd)
{
	ASSERT(hInstance != nullptr);

	spInstance = new GameCore(hInstance);

	ShowWindow(spInstance->mhWnd, nShowCmd);
	UpdateWindow(spInstance->mhWnd);

	GameCore& core = *spInstance;
	core.mpActor = new Actor("Actor");
	MeshComponent* const pMeshComponent = new MeshComponent(core.mpActor);

	core.mpCameraActor = new Actor("MainCamera");
	CameraComponent* const pCameraComponent = new CameraComponent(core.mpCameraActor);
	CameraControllerComponent* const pCameraControllerComponent = new CameraControllerComponent(core.mpCameraActor);

	return SUCCEEDED(spInstance->mErrorCode);
}

LRESULT GameCore::wndProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
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
		if (wParam != SIZE_MINIMIZED)
		{
			Renderer* pRenderer = Renderer::GetInstance();
			ASSERT(pRenderer != nullptr);

			const int width = LOWORD(lParam);
			const int height = HIWORD(lParam);

			pRenderer->OnResize(width, height);
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F4:
			{
				Renderer* pRenderer = Renderer::GetInstance();
				ASSERT(pRenderer != nullptr);

				pRenderer->SwitchWireframeMode();
			}
			break;

		default:
			mbKeyPressed[wParam] = true;
			break;
		}
		break;

	case WM_KEYUP:
		mbKeyPressed[wParam] = false;
		break;

	case WM_MOUSEMOVE:
		{
			mMousePosition.x = LOWORD(lParam);
			mMousePosition.y = HIWORD(lParam);
		}
		break;

	case WM_MBUTTONDOWN:
		mbKeyPressed[VK_MBUTTON] = true;
		break;

	case WM_MBUTTONUP:
		mbKeyPressed[VK_MBUTTON] = false;
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (GameCore::spInstance == nullptr)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return GameCore::spInstance->wndProc(hWnd, msg, wParam, lParam);
}