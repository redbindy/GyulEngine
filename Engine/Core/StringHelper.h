#pragma once

#pragma once

#include <stringapiset.h>

#include "Assert.h"

enum
{
	STRING_BUFFER_SIZE = 256
};

inline void ConvertWideToMulti(char outBuffer[], const TCHAR* const pWide, const int wideLength)
{
	ASSERT(outBuffer != nullptr);
	ASSERT(pWide != nullptr);
	ASSERT(wideLength > 0);

	const int byteLength = wideLength * sizeof(TCHAR);

	WideCharToMultiByte(CP_ACP, 0, pWide, -1, outBuffer, byteLength, nullptr, nullptr);
	outBuffer[byteLength] = '\0';
}

inline void ConvertWideToMulti(char outBuffer[], const TCHAR* const pWide)
{
	ASSERT(outBuffer != nullptr);
	ASSERT(pWide != nullptr);

	const int length = lstrlen(pWide);

	ConvertWideToMulti(outBuffer, pWide, length);
}

inline void ConvertMultiToWide(TCHAR outBuffer[], const char* const pMulti, const int multiLength)
{
	ASSERT(outBuffer != nullptr);
	ASSERT(pMulti != nullptr);
	ASSERT(multiLength > 0);

	MultiByteToWideChar(CP_ACP, 0, pMulti, -1, outBuffer, multiLength);
	outBuffer[multiLength] = '\0';
}

inline void ConvertMultiToWide(TCHAR outBuffer[], const char* const pMulti)
{
	ASSERT(outBuffer != nullptr);
	ASSERT(pMulti != nullptr);

	const int multiLength = static_cast<int>(strlen(pMulti));

	ConvertMultiToWide(outBuffer, pMulti, multiLength);
}

inline void GetExtension(const std::string& path, char outBuffer[])
{
	ASSERT(outBuffer != nullptr);

	const size_t dotPosition = path.find_last_of('.');

	if (dotPosition != std::string::npos)
	{
		strcpy(outBuffer, path.c_str() + dotPosition + 1);
	}
	else
	{
		outBuffer[0] = '\0';
	}
}