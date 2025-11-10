#include "GameCore.h"

#include "UI/ImGuiHeaders.h"

#include "LogHelper.h"
#include "FileDialog.h"

#include "Renderer/Renderer.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Actor.h"
#include "Scene/Components/ComponentFactory.h"
#include "InputSystem.h"

enum
{
	DEFAULT_BUFFER_SIZE = 8
};

static const TCHAR* const CLASS_NAME = TEXT("GyulEngine");

static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

GameCore* GameCore::spInstance = nullptr;

GameCore::GameCore(const HINSTANCE hInstance, const HWND hWnd, const UINT targetFrameRate)
	: mhInstance(hInstance)
	, mhWnd(hWnd)
	, mbPlaying(false)
	, mTargetFrameRate(targetFrameRate)
	, mpCurrentScene(nullptr)
	, mpEditorCameraActor(nullptr)
{
	ASSERT(hInstance != nullptr);
	ASSERT(hWnd != nullptr);
}

GameCore::~GameCore()
{
	SceneManager::Destroy();
	InputSystem::Destroy();
	Renderer::Destroy();
	FileDialog::Destroy();

	UnregisterClass(CLASS_NAME, mhInstance);
}

int GameCore::Run() const
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
			const float frameTime = 1.f / static_cast<float>(mTargetFrameRate);

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

			mpCurrentScene->Update(deltaTime);

			if (!mbPlaying)
			{
				mpEditorCameraActor->Update(deltaTime);
			}

			InputSystem& inputSystem = InputSystem::GetInstance();
			inputSystem.ClearDelta();

			// render
			Renderer& renderer = Renderer::GetInstance();

			renderer.BeginFrame();
			{
				mpCurrentScene->Render();

				renderer.Render();

				renderer.BeginUIFrame();
				{
					ImGui::ShowDemoWindow();
				}
				renderer.EndUIFrame();
			}
			renderer.EndFrame();
		}
	}

	return (int)msg.wParam;
}

bool GameCore::TryInitialize(const HINSTANCE hInstance, const int nShowCmd)
{
	ASSERT(hInstance != nullptr);
	ASSERT(spInstance == nullptr);

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));

	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = CLASS_NAME;

	if (!RegisterClassEx(&wc))
	{
		const int errorCode = GetLastError();
		LOG_SYSTEM_ERROR(errorCode, "RegisterClassEx");

		ASSERT(false);

		return false;
	}

	const HWND hWnd = CreateWindowEx(
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

	if (hWnd == nullptr)
	{
		const int errorCode = GetLastError();
		LOG_SYSTEM_ERROR(errorCode, "CreateWindowEx");

		ASSERT(false);

		return false;
	}

	if (!FileDialog::TryInitialize(hWnd))
	{
		ASSERT(false);

		return false;
	}

	if (!Renderer::TryInitialize(hWnd))
	{
		ASSERT(false);

		return false;
	}

	InputSystem::Initialize();
	SceneManager::Initialize();

	Renderer& renderer = Renderer::GetInstance();

	spInstance = new GameCore(hInstance, hWnd, renderer.GetRefreshRate());
	spInstance->mpCurrentScene = SceneManager::GetInstance().GetScene(0);

	std::unique_ptr<Actor> pEditorCameraActor = std::make_unique<Actor>(
		spInstance->mpCurrentScene,
		"EditorCamera"
	);

	ComponentFactory& componentFactory = ComponentFactory::GetInstance();
	componentFactory.CreateComponentAlloc("CameraComponent", pEditorCameraActor.get());
	componentFactory.CreateComponentAlloc("CameraControllerComponent", pEditorCameraActor.get());

	spInstance->mpEditorCameraActor = std::move(pEditorCameraActor);

	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	return true;
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
			Renderer& renderer = Renderer::GetInstance();

			const int width = LOWORD(lParam);
			const int height = HIWORD(lParam);

			renderer.OnResize(width, height);
		}
		break;

	case WM_MOUSEMOVE:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			const WORD xPos = LOWORD(lParam);
			const WORD yPos = HIWORD(lParam);

			inputSystem.SetMousePosition(xPos, yPos);
		}
		break;

	case WM_KEYDOWN:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			inputSystem.OnKeyDown(static_cast<uint8_t>(wParam));
		}
		break;

	case WM_KEYUP:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			inputSystem.OnKeyUp(static_cast<uint8_t>(wParam));
		}
		break;

	case WM_LBUTTONDOWN:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			inputSystem.OnKeyDown(VK_LBUTTON);
		}
		break;

	case WM_LBUTTONUP:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			inputSystem.OnKeyUp(VK_LBUTTON);
		}
		break;

	case WM_MBUTTONDOWN:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			inputSystem.OnKeyDown(VK_MBUTTON);
		}
		break;

	case WM_MBUTTONUP:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			inputSystem.OnKeyUp(VK_MBUTTON);
		}
		break;

	case WM_RBUTTONDOWN:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			inputSystem.OnKeyDown(VK_RBUTTON);
		}
		break;

	case WM_RBUTTONUP:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			inputSystem.OnKeyUp(VK_RBUTTON);
		}
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (GameCore::spInstance == nullptr)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return GameCore::spInstance->wndProc(hWnd, msg, wParam, lParam);
}
