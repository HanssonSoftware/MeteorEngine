/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Utils.h"
#include <Types/String.h>

#include "Windows/WindowsPaths.h"
#include <PathCch.h>

#include <shlwapi.h>
#include <strsafe.h>
#pragma comment(lib, "Shlwapi.lib")


LOG_ADDCATEGORY(BuildSystemUtils);

String Utils::GetLastError()
{ 
	//const DWORD error = ;
	LPWSTR buffer = nullptr;

	const uint32_t returned = (uint32_t)FormatMessageW(
	/* DWORD dwFlags */			FORMAT_MESSAGE_ALLOCATE_BUFFER |
								FORMAT_MESSAGE_FROM_SYSTEM |
								FORMAT_MESSAGE_IGNORE_INSERTS |
								FORMAT_MESSAGE_MAX_WIDTH_MASK,
	/* LPCVOID lpSource */		nullptr,
	/* DWORD   dwMessageId */	::GetLastError(),
	/* DWORD   dwLanguageId */	LANG_USER_DEFAULT,
	/* LPWSTR  lpBuffer */		(LPWSTR)&buffer,
	/* DWORD   nSize */			0,
	/* va_list *Arguments */	nullptr
	);

	if (buffer)
	{

	}

	return "";
}

void Utils::ListDirectory(wchar_t* name, wchar_t*& container)
{
	if (name != nullptr)
	{
		if (PathIsRelativeW(name))
		{
			wchar_t exeDir[512] = {};
			if (!GetModuleFileNameW(nullptr, exeDir, 512))
			{
				MR_LOG(LogBuildSystemUtils, Error, "GetModuleFileNameW returned: %s", *Utils::GetLastError());
				return;
			}

			DWORD length = (DWORD)wcslen(exeDir);
			PathCchRemoveFileSpec(exeDir, length);

			PWSTR combinedPathNonCanonicalized = nullptr;
			PathAllocCombine(exeDir, name, PATHCCH_ALLOW_LONG_PATHS, &combinedPathNonCanonicalized);

			wcscpy(name, combinedPathNonCanonicalized);
			LocalFree(combinedPathNonCanonicalized);
		}

		wcscat(name, L"\\*");

		WIN32_FIND_DATAW foundFile;
		HANDLE fileHandle = FindFirstFileW(name, &foundFile);

		PathCchRemoveFileSpec(name, wcslen(name));
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (foundFile.cFileName[0] == L'.')
					continue;

				if (foundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					wchar_t* tempName = new wchar_t[wcslen(name) + wcslen(foundFile.cFileName) + 10]();
					StringCchPrintfW(tempName, STRSAFE_MAX_CCH, L"%ls\\%ls", name, foundFile.cFileName);
					ListDirectory(tempName, container);

					delete[] tempName;
				}
				else if (foundFile.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
				{
					wchar_t* tempName = new wchar_t[wcslen(name) + wcslen(foundFile.cFileName) + 10]();
					StringCchPrintfW(tempName, STRSAFE_MAX_CCH, L"%ls\\%ls", name, foundFile.cFileName);
					//container.Add(tempName);
				
					delete[] tempName;
				}

			} while (FindNextFileW(fileHandle, &foundFile));
		
			FindClose(fileHandle);
		}
	}
}

