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

void LogSystemError(const long errorCode, const char* const msg);