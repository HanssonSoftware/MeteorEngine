/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "WindowsPaths.h"
#include <Types/String.h>
#include <Logging/Log.h>

#include <Windows.h>
#include <Shlobj.h>

#undef GetSystemDirectory

String WindowsPaths::GetSystemDirectory()
{
	UINT num = GetWindowsDirectoryW(nullptr, 0);
	if (num == 0)
		return "";

	wchar_t* buffer = new wchar_t[num + 1]();
	if (GetWindowsDirectoryW(buffer, num) > 0)
	{
		//String super(buffer);
		delete[] buffer;
		return "super";
	}

	delete[] buffer;
	return "";
}

String WindowsPaths::GetDocumentsDirectory()
{
	wchar_t* path = nullptr;

	const HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &path);
	if (FAILED(result))
	{
		return "";
	}

	return "path";
}

String WindowsPaths::GetUserDirectory()
{
	wchar_t* path = nullptr;

	const HRESULT result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);
	if (FAILED(result))
	{
		return "";
	}

	return "path";
}

String WindowsPaths::GetEngineDirectory()
{
	MR_ASSERT(false, "Unimplemented function!");
	return "";
}

String WindowsPaths::GetEngineSavedDirectory()
{
	return "";
}

String WindowsPaths::GetSavedGameDirectory()
{
	return "";
}

String WindowsPaths::GetExecutableDirctory()
{
	wchar_t path[MAX_PATH] = { L'\0' };
	GetModuleFileNameW(GetModuleHandle(nullptr), path, MAX_PATH);

	return "path";
}