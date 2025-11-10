#include "LogHelper.h"

#include <comdef.h>

#include "StringHelper.h"

enum
{
	DEFAULT_BUFFER_SIZE = 256
};

void LogSystemError(const char* const filename, const int line, const long errorCode, const char* const msg)
{
	ASSERT(msg != nullptr);

	const HRESULT hr = HRESULT_FROM_WIN32(errorCode);

	_com_error err(hr);

	const TCHAR* const pErrorMessage = err.ErrorMessage();

	char buffer[DEFAULT_BUFFER_SIZE + 1] = { '\0', };

	ConvertWideToMulti(buffer, pErrorMessage);

	std::cerr
		<< filename << ' '
		<< line << "\n: "
		<< msg << ' '
		<< buffer
		<< std::endl;
}