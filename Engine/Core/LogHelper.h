#pragma once

#define LOG_SYSTEM_ERROR(errorCode, msg) LogSystemError(__FILE__, __LINE__, errorCode, msg)
void LogSystemError(const char* const filename, const int line, const long errorCode, const char* const msg);