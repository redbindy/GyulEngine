#pragma once

#include <Windows.h>

class App final
{
public:
	App();
	virtual ~App();

	void Initialize(const HWND hWnd);

	void Run();

private:
	HWND mhWnd;

private:
	App(const App& other) = delete;
	App& operator=(const App& other) = delete;
	App(App&& other) noexcept = delete;
	App& operator=(App&& other) noexcept = delete;
};