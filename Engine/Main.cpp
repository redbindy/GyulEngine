#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#include <crtdbg.h>
#endif

#include <Windows.h>
#include <Core/GameCore.h>

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_     PWSTR     pCmdLine,
	_In_     int       nShowCmd
)
{
	int code = 0;
	if (!GameCore::TryInitialize(hInstance, nShowCmd))
	{
		MessageBox(nullptr, TEXT("Failed to initialize"), TEXT("Error"), MB_OK | MB_ICONERROR);

		code = -1;
	}
	else
	{
		GameCore& gameCore = GameCore::GetInstance();
		{
			code = gameCore.Run();
		}
		gameCore.Destroy();
	}

#if defined(DEBUG) || defined(_DEBUG)
	_CrtDumpMemoryLeaks();
#endif

	return code;
}