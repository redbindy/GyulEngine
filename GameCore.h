#pragma once

#include <Windows.h>

#include "SimpleMath.h"

#include "DebugHelper.h"
#include "IUIDrawable.h"

using namespace DirectX;
using namespace SimpleMath;

class Actor;
class InteractionCollider;

class GameCore final : public IUIDrawable
{
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	GameCore(const GameCore& other) = delete;
	GameCore& operator=(const GameCore& other) = delete;
	GameCore(GameCore&& other) = delete;
	GameCore& operator=(GameCore&& other) = delete;

	int Run();

	void RegisterInteraction(Actor* const pOwner, const BoundingSphere& boundingSphereLocal);
	void UnregisterInteraction(const Actor* const pOwner);

	static bool TryInitialize(const HINSTANCE hInstance, const int nShowCmd);

	inline static GameCore& GetInstance()
	{
		ASSERT(spInstance != nullptr, "Please call Initialize first");

		return *spInstance;
	}

	inline static void Destroy()
	{
		if (spInstance == nullptr)
		{
			return;
		}

		delete spInstance;
	}

	inline HRESULT GetErrorCode() const
	{
		return mErrorCode;
	}

	inline bool IsKeyPressed(const uint8_t keyCode) const
	{
		return mbKeyPressed[keyCode];
	}

	inline Vector2 GetMousePosition() const
	{
		return mMousePosition;
	}

	inline float GetMouseX() const
	{
		return mMousePosition.x;
	}

	inline float GetMouseY() const
	{
		return mMousePosition.y;
	}

	inline void SuspendUpdates()
	{
		mbSuspending = true;
	}

	inline void ResumeUpdates()
	{
		mbSuspending = false;
	}

protected:
	virtual void DrawUI() override;

private:
	static GameCore* spInstance;

	HINSTANCE mhInstance;
	HWND mhWnd;
	HRESULT mErrorCode;

	bool mbKeyPressed[UINT8_MAX];

	Vector2 mMousePosition;

	std::vector<Actor*> mActors;
	std::vector<Actor*> mPendingActors;
	bool mbSuspending;

	std::vector<InteractionCollider> mInteractionColliders;

private:
	GameCore(const HINSTANCE hInstance);
	~GameCore();

	LRESULT CALLBACK wndProc(const HWND hWnd, const UINT msg, const WPARAM wParam, const LPARAM lParam);

	void onAddActor();
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);