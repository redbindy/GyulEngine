#pragma once

#include <Windows.h>

#include "DebugHelper.h"

enum
{
	STRING_BUFFER_SIZE = 256
};

inline void ConvertWideToMulti(char buffer[], const TCHAR* const pWide, const int wideLength)
{
	ASSERT(buffer != nullptr);
	ASSERT(pWide != nullptr);
	ASSERT(wideLength > 0);

	const int byteLength = wideLength * sizeof(TCHAR);

	WideCharToMultiByte(CP_ACP, 0, pWide, -1, buffer, byteLength, nullptr, nullptr);
	buffer[byteLength] = '\0';
}

inline void ConvertWideToMulti(char buffer[], const TCHAR* const pWide)
{
	ASSERT(buffer != nullptr);
	ASSERT(pWide != nullptr);

	const int length = lstrlen(pWide);

	ConvertWideToMulti(buffer, pWide, length);
}