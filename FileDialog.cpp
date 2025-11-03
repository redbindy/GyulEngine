#include "FileDialog.h"

#include "DebugHelper.h"
#include "ComHelper.h"
#include "CommonDefs.h"
#include "StringHelper.h"

FileDialog* FileDialog::staticInstance = nullptr;

FileDialog::FileDialog(const HWND hWnd)
	: mhOwnerWnd(hWnd)
	, mpOpenDialog(nullptr)
{
	// com init
	HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "CoInitializeEx");

		ASSERT(false);
	}

	hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&mpOpenDialog));

	if (FAILED(hr))
	{
		LOG_SYSTEM_ERROR(hr, "CoCreateInstance");

		ASSERT(false);
	}

	COMDLG_FILTERSPEC filterSpecs[] =
	{
		{ TEXT("Image"), TEXT("*.jpg;*.jpeg;*.png;*.gif;*.bmp") },
		{ TEXT("All"), TEXT("*.*") }
	};

	mpOpenDialog->SetFileTypes(ARRAYSIZE(filterSpecs), filterSpecs);
}

FileDialog::~FileDialog()
{
	SafeRelease(mpOpenDialog);

	CoUninitialize();
}

void FileDialog::CreateInstance(const HWND hWnd)
{
	if (staticInstance != nullptr)
	{
		delete staticInstance;
	}

	staticInstance = new FileDialog(hWnd);
}

FileDialog& FileDialog::GetInstance()
{
	ASSERT(staticInstance != nullptr, "Should call CreateInstance() calling GetInstance()");

	return *staticInstance;
}

void FileDialog::Destroy()
{
	delete staticInstance;
}

bool FileDialog::TryOpenFileDialog(char outFilePath[], const int bufferLength)
{
	ASSERT(outFilePath != nullptr);
	ASSERT(bufferLength > 0);

	HRESULT hr = mpOpenDialog->Show(mhOwnerWnd);
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