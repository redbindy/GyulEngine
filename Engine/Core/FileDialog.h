#pragma once

#include <ShObjIdl_core.h>

#include "Assert.h"

class FileDialog final
{
public:
	enum
	{
		PATH_BUFFER_SIZE = MAX_PATH * 4
	};

public:
	bool TryOpenFileDialog(char outFilePath[], const int bufferLength) const;

	// static
	static bool TryInitialize(const HWND hWnd);

	static inline FileDialog& GetInstance()
	{
		ASSERT(spInstance != nullptr);

		return *spInstance;
	}

	static inline void Destroy()
	{
		delete spInstance;
	}

private:
	static FileDialog* spInstance;

	HWND mhWnd;
	IFileOpenDialog* mpOpenDialog;

private:
	FileDialog(const HWND hWnd, IFileOpenDialog* const pOpenDialog);
	~FileDialog();

private:
	FileDialog(const FileDialog& other) = delete;
	FileDialog(FileDialog&& other) = delete;
	FileDialog& operator=(const FileDialog& other) = delete;
	FileDialog& operator=(FileDialog&& other) = delete;
};