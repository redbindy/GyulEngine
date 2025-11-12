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

#include "Resources/MeshManager.h"
#include "Resources/MaterialManager.h"
#include "Resources/ShaderManager.h"
#include "Resources/TextureManager.h"
#include "InteractionSystem.h"

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
	, mpEditorCameraComponent(nullptr)
	, mbShowResources(true)
	, mbShowSettingsPopup(false)
{
	ASSERT(hInstance != nullptr);
	ASSERT(hWnd != nullptr);
}

GameCore::~GameCore()
{
	SceneManager::Destroy();
	InteractionSystem::Destroy();
	InputSystem::Destroy();
	Renderer::Destroy();
	FileDialog::Destroy();

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

			if (!mbPlaying)
			{
				mpEditorCameraActor->Update(deltaTime);

				InteractionSystem& interactionSystem = InteractionSystem::GetInstance();
				interactionSystem.Update();
			}
			else
			{
				mpCurrentScene->Update(deltaTime);
			}

			InputSystem& inputSystem = InputSystem::GetInstance();
			inputSystem.ClearDelta();

			// render
			Renderer& renderer = Renderer::GetInstance();

			renderer.BeginFrame();
			{
				renderer.RenderScene(mpCurrentScene->GetName());

				if (!mbPlaying)
				{
					renderer.BeginUIFrame();

					// Global editor UI
					DrawEditorUI();

					// readonly editor camera UI
					// ImGui::BeginDisabled();
					{
						mpEditorCameraActor->DrawEditorUI();
					}
					// ImGui::EndDisabled();

					if (mpCurrentScene != nullptr)
					{
						mpCurrentScene->DrawEditorUI();
					}

					renderer.EndUIFrame();
				}
			}
			renderer.EndFrame();
		}
	}

	return (int)msg.wParam;
}

void GameCore::DrawEditorUI()
{
	// Use class name as ID scope
	ImGui::PushID("GameCore");

	// Main menu bar with File -> Exit (localized)
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(UTF8_TEXT("파일")))
		{
			if (ImGui::MenuItem(UTF8_TEXT("종료"), "Alt+F4"))
			{
				PostQuitMessage(0);
			}
			ImGui::EndMenu();
		}

		// 보기 메뉴: 리소스 창 토글
		if (ImGui::BeginMenu(UTF8_TEXT("보기")))
		{
			ImGui::MenuItem(UTF8_TEXT("리소스 창"), nullptr, &mbShowResources);
			ImGui::EndMenu();
		}

		ImGui::MenuItem(UTF8_TEXT("모드 변경(F5)"), nullptr, nullptr, false);

		ImGui::EndMainMenuBar();
	}

	// FPS overlay (localized labels, keep window ID string for stability)
	{
		const ImGuiViewport* vp = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + 10.f, vp->WorkPos.y + 10.f), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		if (ImGui::Begin(UTF8_TEXT("FPS"), nullptr, flags))
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("FPS: %.1f", io.Framerate); // Keep acronym as-is
		}
		ImGui::End();
	}

	// 하단 리소스 탭 영역 - 우선 씬 매니저 탭만 구현
	if (mbShowResources)
	{
		const ImGuiViewport* vp = ImGui::GetMainViewport();
		const float resourceHeight = 180.f; // 기본 높이
		ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x, vp->WorkPos.y + vp->WorkSize.y - resourceHeight), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(vp->WorkSize.x, resourceHeight));

		ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus;
		if (ImGui::Begin(UTF8_TEXT("리소스"), nullptr, winFlags))
		{
			if (ImGui::BeginTabBar("ResourceTabs"))
			{
				if (ImGui::BeginTabItem(UTF8_TEXT("씬")))
				{
					SceneManager::GetInstance().DrawEditorUI();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(UTF8_TEXT("메쉬")))
				{
					MeshManager::GetInstance().DrawEditorUI();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(UTF8_TEXT("머티리얼")))
				{
					MaterialManager::GetInstance().DrawEditorUI();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(UTF8_TEXT("셰이더")))
				{
					ShaderManager::GetInstance().DrawEditorUI();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(UTF8_TEXT("텍스처")))
				{
					TextureManager::GetInstance().DrawEditorUI();
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	}

	const char* const settingsId = UTF8_TEXT("설정");
	if (mbShowSettingsPopup)
	{
		ImGui::OpenPopup(settingsId, ImGuiPopupFlags_NoReopen);
	}
	else
	{
		ImGui::CloseCurrentPopup();
	}

	if (ImGui::BeginPopupModal(settingsId, &mbShowSettingsPopup, ImGuiWindowFlags_AlwaysAutoResize))
	{
		Renderer& renderer = Renderer::GetInstance();

		// GameCore target frame rate slider
		int frameRate = static_cast<int>(mTargetFrameRate);
		if (ImGui::SliderInt(UTF8_TEXT("목표 프레임레이트"), &frameRate, 30, renderer.GetRefreshRate()))
		{
			mTargetFrameRate = static_cast<UINT>(frameRate);
		}

		// Renderer settings
		renderer.DrawEditorUI();

		ImGui::EndPopup();
	}

	ImGui::PopID();
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
	InteractionSystem::Initialize();
	SceneManager::Initialize();

	Renderer& renderer = Renderer::GetInstance();
	SceneManager& sceneManager = SceneManager::GetInstance();

	spInstance = new GameCore(hInstance, hWnd, renderer.GetRefreshRate());
	spInstance->mpCurrentScene = sceneManager.GetScene("Default");

	std::unique_ptr<Actor> pEditorCameraActor = std::make_unique<Actor>(
		spInstance->mpCurrentScene,
		"EditorCamera"
	);

	ComponentFactory& componentFactory = ComponentFactory::GetInstance();

	Component* const pCameraComponent = componentFactory.CreateComponentAlloc("CameraComponent", pEditorCameraActor.get());
	Component* const pCameraControllerComponent = componentFactory.CreateComponentAlloc("CameraControllerComponent", pEditorCameraActor.get());

	spInstance->mpEditorCameraComponent = reinterpret_cast<CameraComponent*>(pCameraComponent);
	spInstance->mpEditorCameraActor = std::move(pEditorCameraActor);

	renderer.SetEditorCameraComponent(spInstance->mpEditorCameraComponent);

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
			switch (wParam)
			{
			case VK_ESCAPE:
				mbShowSettingsPopup = !mbShowSettingsPopup;
				break;

			case VK_F5:
				mbPlaying = !mbPlaying;
				if (mbPlaying)
				{
					mpCurrentScene->EnterPlayMode();
				}
				else
				{
					mpCurrentScene->ExitPlayMode();

					Renderer& renderer = Renderer::GetInstance();

					renderer.SetMainCameraComponent(mpEditorCameraComponent);
				}
				break;

			default:
				break;
			}

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

			if (!mbPlaying)
			{
				InteractionSystem& interactionSystem = InteractionSystem::GetInstance();

				interactionSystem.Pick(mpCurrentScene->GetName());
			}
		}
		break;

	case WM_LBUTTONUP:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			inputSystem.OnKeyUp(VK_LBUTTON);

			if (!mbPlaying)
			{
				InteractionSystem& interactionSystem = InteractionSystem::GetInstance();

				interactionSystem.ReleasePick();
			}
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

			if (!mbPlaying)
			{
				InteractionSystem& interactionSystem = InteractionSystem::GetInstance();

				interactionSystem.Pick(mpCurrentScene->GetName());
			}
		}
		break;

	case WM_RBUTTONUP:
		{
			InputSystem& inputSystem = InputSystem::GetInstance();

			inputSystem.OnKeyUp(VK_RBUTTON);

			if (!mbPlaying)
			{
				InteractionSystem& interactionSystem = InteractionSystem::GetInstance();

				interactionSystem.ReleasePick();
			}
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
