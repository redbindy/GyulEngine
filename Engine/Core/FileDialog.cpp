#include "FileDialog.h"

#include "Assert.h"
#include "LogHelper.h"
#include "ComHelper.h"
#include "StringHelper.h"

FileDialog* FileDialog::spInstance = nullptr;

FileDialog::FileDialog(const HWND hWnd, IFileOpenDialog* const pOpenDialog)
	: mhWnd(hWnd)
	, mpOpenDialog(pOpenDialog)
{
	ASSERT(hWnd != nullptr);
	ASSERT(pOpenDialog != nullptr);
}

FileDialog::~FileDialog()
{
	SafeRelease(mpOpenDialog);

	CoUninitialize();
}

bool FileDialog::TryInitialize(const HWND hWnd)
{
	ASSERT(hWnd != nullptr);
	ASSERT(spInstance == nullptr);

	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "CoInitializeEx");

		ASSERT(false);

		return false;
	}

	IFileOpenDialog* pOpenDialog = nullptr;
	hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pOpenDialog));

	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "CoCreateInstance");

		ASSERT(false);

		return false;
	}

	const COMDLG_FILTERSPEC filterSpecs[] =
	{
		{ TEXT("Image"), TEXT("*.jpg;*.jpeg;*.png;*.gif;*.bmp") },
		{ TEXT("All"), TEXT("*.*") }
	};

	pOpenDialog->SetFileTypes(ARRAYSIZE(filterSpecs), filterSpecs);

	spInstance = new FileDialog(hWnd, pOpenDialog);

	return true;
}

bool FileDialog::TryOpenFileDialog(char outFilePath[], const int bufferLength) const
{
	ASSERT(outFilePath != nullptr);
	ASSERT(bufferLength > 0);

	HRESULT hr = mpOpenDialog->Show(mhWnd);
	if (SUCCEEDED(hr))
	{
		IShellItem* pItem = nullptr;
		hr = mpOpenDialog->GetResult(&pItem);

		if (SUCCEEDED(hr))
		{
			TCHAR* pFilePath = nullptr;
			hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath);

			if (FAILED(hr))
			{
				LOG_SYSTEM_ERROR(hr, "GetDisplayName");

				ASSERT(false);

				return false;
			}

			ASSERT(pFilePath != nullptr);
			{
				const int filePathLength = static_cast<int>(wcslen(pFilePath));
				ASSERT(filePathLength + 1 <= bufferLength);

				ConvertWideToMulti(outFilePath, pFilePath, filePathLength + 1);
			}
			CoTaskMemFree(pFilePath);
		}
		SafeRelease(pItem);
	}

	return SUCCEEDED(hr);
}