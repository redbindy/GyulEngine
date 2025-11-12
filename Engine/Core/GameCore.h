#pragma once

#include <vector>
#include <wtypes.h>
#include <memory>

#include "Assert.h"
#include "UI/IEditorUIDrawable.h"

class Scene;
class Actor;
class CameraComponent;

class GameCore final : public IEditorUIDrawable
{
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	friend LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	int Run();

	virtual void DrawEditorUI() override;

	inline void SetCurrentScene(Scene* const pScene)
	{
		ASSERT(pScene != nullptr);

		mpCurrentScene = pScene;
	}

	// static
	static bool TryInitialize(const HINSTANCE hInstance, const int nShowCmd);

	static inline GameCore& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static inline void Destroy()
	{
		delete spInstance;
	}

private:
	static GameCore* spInstance;

	HINSTANCE mhInstance;
	HWND mhWnd;

	bool mbPlaying;

	UINT mTargetFrameRate;

	Scene* mpCurrentScene;

	std::unique_ptr<Actor> mpEditorCameraActor;
	CameraComponent* mpEditorCameraComponent;

	bool mbShowResources; // whether resource window is visible
	bool mbShowSettingsPopup; // whether settings popup is visible

private:
	GameCore(const HINSTANCE hInstance, const HWND hWnd, const UINT targetFrmaeRate);
	~GameCore();

	LRESULT CALLBACK wndProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam);

private:
	GameCore(const GameCore& other) = delete;
	GameCore& operator=(const GameCore& other) = delete;
	GameCore(GameCore&& other) = delete;
	GameCore& operator=(GameCore&& other) = delete;
};