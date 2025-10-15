#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#include <crtdbg.h>
#endif

#include <Windows.h>

#include "GameCore.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     PWSTR     pCmdLine,
	_In_     int       nShowCmd
)
{
	int exitCode = 0;

	const bool bResult = GameCore::TryInitialize(hInstance, nShowCmd);
	{
		if (!bResult)
		{
			MessageBox(nullptr, TEXT("Failed to initilize"), TEXT("Error"), MB_OK);
		}
		else
		{
			GameCore* const pGameCore = GameCore::GetInstance();

			exitCode = pGameCore->Run();
		}
	}
	GameCore::Destroy();

#if defined(DEBUG) || defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif

	return exitCode;
}