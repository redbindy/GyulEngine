#pragma once

#include <vector>
#include <wtypes.h>
#include <memory>

#include "Assert.h"

class Scene;
class Actor;

class GameCore final
{
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	friend LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	int Run() const;

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