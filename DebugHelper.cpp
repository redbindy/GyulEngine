#include "DebugHelper.h"

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
