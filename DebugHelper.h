#pragma once

#include <cassert>
#include <iostream>
#include <Windows.h>
#include <comdef.h>

#if defined(_DEBUG) || defined(DEBUG)

#define ASSERT_DEFAULT(expr) \
    if (!(expr))             \
    {                        \
        DebugBreak();        \
        std::terminate();    \
    }                        \

#define ASSERT_WITH_MESSAGE(expr, msg)       \
    if (!(expr))                         \
    {                                    \
        std::cerr << __FILE__ << ' ' << __LINE__ << "\n: " << (msg) << std::endl; \
        DebugBreak();                    \
        std::terminate();                \
    }                                    \

#define EXPAND(x) x
#define VA_GENERIC(_1, _2, x, ...) x
#define ASSERT(...) EXPAND(VA_GENERIC(__VA_ARGS__, ASSERT_WITH_MESSAGE, ASSERT_DEFAULT)(__VA_ARGS__))

#else
#define ASSERT
#endif /* DEBUG */

enum
{
	DEFAULT_BUFFER_SIZE = 256
};

void LogSystemError(const long errorCode, const char* const msg)
{
	ASSERT(msg != nullptr);

	const HRESULT hr = HRESULT_FROM_WIN32(errorCode);

	_com_error err(hr);

	const TCHAR* const pErrorMessage = err.ErrorMessage();

	const int byteLength = lstrlen(pErrorMessage) * sizeof(TCHAR);
	char buffer[DEFAULT_BUFFER_SIZE + 1] = { '\0', };

	WideCharToMultiByte(CP_ACP, 0, pErrorMessage, -1, buffer, byteLength, nullptr, nullptr);
	buffer[byteLength] = '\0';

	std::cerr << msg << ' ' << buffer << std::endl;
}