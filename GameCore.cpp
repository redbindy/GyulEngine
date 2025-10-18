#include "GameCore.h"

#include <algorithm>

#include "Renderer/Renderer.h"
#include "Actor.h"
#include "Component/MeshComponent.h"
#include "Component/CameraComponent.h"
#include "Component/CameraControllerComponent.h"

enum
{
	DEFAULT_BUFFER_SIZE = 256
};

GameCore* GameCore::spInstance = nullptr;

static const TCHAR* const CLASS_NAME = TEXT("GyulEngine");

GameCore::GameCore(const HINSTANCE hInstance)
	: mhInstance(hInstance)
	, mhWnd(nullptr)
	, mErrorCode(S_OK)
	, mbKeyPressed{ false, }
	, mActors()
	, mPendingActors()
	, mPickingListeners()
{
	ASSERT(hInstance != nullptr);

	mActors.reserve(DEFAULT_BUFFER_SIZE);
	mPendingActors.reserve(DEFAULT_BUFFER_SIZE);
	mPickingListeners.reserve(DEFAULT_BUFFER_SIZE);

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
	for (Actor* const pActor : mActors)
	{
		delete pActor;
	}

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
			Renderer& renderer = Renderer::GetInstance();

			const UINT targetFrameRate = renderer.GetTargetFrameRate();
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

			if (!mbSuspending)
			{
				for (Actor* const pActor : mActors)
				{
					pActor->Update(deltaTime);
				}
			}

			// hovering
			Ray ray;
			if (renderer.TryGetMouseRay(mMousePosition, ray))
			{
				IPickingListener* pPickedListener = nullptr;
				float dist = D3D11_FLOAT32_MAX;

				for (IPickingListener* const pListener : mPickingListeners)
				{
					float collisionDist = D3D11_FLOAT32_MAX;

					if (pListener->CheckCollision(ray, collisionDist) && collisionDist < dist)
					{
						dist = collisionDist;

						pPickedListener = pListener;
					}
				}

				if (pPickedListener != nullptr)
				{
					pPickedListener->OnCollision();
				}
			}

			// render
			renderer.BeginFrame();
			{
				renderer.RenderScene();

				renderer.BeginUI();
				{
					ImGui::Begin("MainPanel");
					{
						DrawUI();
						renderer.DrawUI();

						constexpr const char* const OBJECTS_LABEL = "Objects";
						if (ImGui::BeginChild(OBJECTS_LABEL, ImVec2(0, 0), ImGuiChildFlags_Border))
						{
							ImGui::Text(OBJECTS_LABEL);
							ImGui::Separator();

							for (Actor* const pActor : mActors)
							{
								pActor->DrawUI();
							}

							ImGui::Separator();

							if (ImGui::Button("AddActor"))
							{
								onAddActor();
							}
						}
						ImGui::EndChild();
					}
					ImGui::End();
				}
				renderer.EndUI();
			}
			renderer.EndFrame();

			// cleanup actors
			for (Actor* const pActor : mActors)
			{
				if (pActor->IsAlive())
				{
					mPendingActors.push_back(pActor);
				}
				else
				{
					delete pActor;
				}
			}

			mActors.swap(mPendingActors);
			mPendingActors.clear();
		}
	}

	return (int)msg.wParam;
}

void GameCore::AddPickingListener(IPickingListener* const pListener)
{
	ASSERT(pListener != nullptr);

	mPickingListeners.push_back(pListener);
}

void GameCore::RemovePickingListener(IPickingListener* const pListener)
{
	ASSERT(pListener != nullptr);

#define VECTOR_ITER std::vector<IPickingListener*>::iterator

	VECTOR_ITER iter = std::find(mPickingListeners.begin(), mPickingListeners.end(), pListener);
	if (iter != mPickingListeners.end())
	{
		mPickingListeners.erase(iter);
	}

#undef VECTOR_ITER
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
			Renderer& renderer = Renderer::GetInstance();

			const int width = LOWORD(lParam);
			const int height = HIWORD(lParam);

			renderer.OnResize(width, height);
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F4:
			{
				Renderer& renderer = Renderer::GetInstance();

				renderer.SwitchWireframeMode();
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

void GameCore::onAddActor()
{
	char actorName[DEFAULT_BUFFER_SIZE];
	sprintf(actorName, "Actor%d", static_cast<int>(mActors.size()));

	Actor* const pActor = new Actor(actorName);
	mActors.push_back(pActor);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (GameCore::spInstance == nullptr)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return GameCore::spInstance->wndProc(hWnd, msg, wParam, lParam);
}