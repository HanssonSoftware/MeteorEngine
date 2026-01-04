/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#define _CRT_SECURE_NO_WARNINGS
#include "Utils.h"
#include <Types/StringW.h>

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
		String final;

		char* finalBuffer = final.Data();
		if (!WideCharToMultiByte(
			/* UINT		CodePage */				CP_UTF8,
			/* DWORD	dwFlags */				WC_NO_BEST_FIT_CHARS,
			/* LPCWCH	lpWideCharStr */		buffer,
			/* int		cchWideChar */			returned,
			/* LPSTR	lpMultiByteStr */		finalBuffer,
			/* int		cbMultiByte */			returned,
			/* LPCCH	lpDefaultChar */		nullptr,
			/* LPBOOL	lpUsedDefaultChar */	nullptr
		))
		{
			switch (::GetLastError())
			{
				case ERROR_INSUFFICIENT_BUFFER:
					MR_LOG(LogBuildSystemUtils, Error, "WideCharToMultiByte returned: ERROR_INSUFFICIENT_BUFFER")
					break;
				case ERROR_INVALID_FLAGS:
					MR_LOG(LogBuildSystemUtils, Error, "WideCharToMultiByte returned: ERROR_INVALID_FLAGS")
					break;
				case ERROR_INVALID_PARAMETER:
					MR_LOG(LogBuildSystemUtils, Error, "WideCharToMultiByte returned: ERROR_INVALID_PARAMETER")
					break;
				case ERROR_NO_UNICODE_TRANSLATION:
					MR_LOG(LogBuildSystemUtils, Error, "WideCharToMultiByte returned: ERROR_NO_UNICODE_TRANSLATION")
					break;

				default:
					MR_LOG(LogBuildSystemUtils, Error, "WideCharToMultiByte returned: Unknown")
					break;
			}

		}

		LocalFree(buffer);
		return final;
	}

	return "";
}

void Utils::ListDirectory(wchar_t* name, Array<StringW>& container)
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
					container.Add(tempName);
				
					delete[] tempName;
				}

			} while (FindNextFileW(fileHandle, &foundFile));
		
			FindClose(fileHandle);
		}
	}
}

