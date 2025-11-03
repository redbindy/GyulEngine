#pragma once

#include <ShObjIdl.h>

class FileDialog final
{
public:
	static void CreateInstance(const HWND hWnd);
	static FileDialog& GetInstance();
	static void Destroy();

	bool TryOpenFileDialog(char outFilePath[], const int bufferLength);

private:
	static FileDialog* staticInstance;

	HWND mhOwnerWnd;
	IFileOpenDialog* mpOpenDialog;

private:
	FileDialog(const HWND hWnd);
	~FileDialog();
	FileDialog(const FileDialog& other) = delete;
	FileDialog(FileDialog&& other) = delete;
	FileDialog& operator=(const FileDialog& other) = delete;
	FileDialog& operator=(FileDialog&& other) = delete;
};