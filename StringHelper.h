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

inline void ConvertMultiToWide(TCHAR buffer[], const char* const pMulti, const int multiLength)
{
	ASSERT(buffer != nullptr);
	ASSERT(pMulti != nullptr);
	ASSERT(multiLength > 0);

	MultiByteToWideChar(CP_ACP, 0, pMulti, -1, buffer, multiLength);
	buffer[multiLength] = '\0';
}

inline void ConvertMultiToWide(TCHAR buffer[], const char* const pMulti)
{
	ASSERT(buffer != nullptr);
	ASSERT(pMulti != nullptr);

	const int multiLength = static_cast<int>(strlen(pMulti));

	ConvertMultiToWide(buffer, pMulti, multiLength);
}