/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Utils.h"
//#include "Parser.h"
#include <Platform/FileManager.h>
#include <Platform/Paths.h>
#include <Types/Pointers.h>
#include <Platform/Platform.h>

#include "Windows/WindowsPaths.h"
#include <PathCch.h>

#include <shlwapi.h>
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

void Utils::ListDirectory(String* name, Array<String>& container)
{
	if (name != nullptr)
	{
		const uint32_t required = MultiByteToWideChar(
			/* UINT		CodePage */				CP_UTF8,
			/* DWORD	dwFlags */				0,
			/* LPSTR	lpMultiByteStr */		**name,
			/* int		cbMultiByte */			-1,
			/* LPCWCH	lpWideCharStr */		nullptr,
			/* int		cchWideChar */			0
		);

		if (required == 0)
		{
			switch (GetLastError())
			{
			case ERROR_INSUFFICIENT_BUFFER:
				MR_LOG(LogBuildSystemUtils, Error, "MultiByteToWideChar returned: ERROR_INSUFFICIENT_BUFFER")
					break;
			case ERROR_INVALID_FLAGS:
				MR_LOG(LogBuildSystemUtils, Error, "MultiByteToWideChar returned: ERROR_INVALID_FLAGS")
					break;
			case ERROR_INVALID_PARAMETER:
				MR_LOG(LogBuildSystemUtils, Error, "MultiByteToWideChar returned: ERROR_INVALID_PARAMETER")
					break;
			case ERROR_NO_UNICODE_TRANSLATION:
				MR_LOG(LogBuildSystemUtils, Error, "MultiByteToWideChar returned: ERROR_NO_UNICODE_TRANSLATION")
					break;

			default:
				MR_LOG(LogBuildSystemUtils, Error, "MultiByteToWideChar returned: Unknown")
					break;
			}

			return;
		}

		wchar_t* buffer = new wchar_t[required];

		if (!MultiByteToWideChar(
			/* UINT		CodePage */				CP_UTF8,
			/* DWORD	dwFlags */				0,
			/* LPSTR	lpMultiByteStr */		**name,
			/* int		cbMultiByte */			name->Length() * sizeof(char),
			/* LPCWCH	lpWideCharStr */		buffer,
			/* int		cchWideChar */			required
		))
		{
			switch (::GetLastError())
			{
				case ERROR_INSUFFICIENT_BUFFER:
					MR_LOG(LogBuildSystemUtils, Error, "MultiByteToWideChar returned: ERROR_INSUFFICIENT_BUFFER")
						break;
				case ERROR_INVALID_FLAGS:
					MR_LOG(LogBuildSystemUtils, Error, "MultiByteToWideChar returned: ERROR_INVALID_FLAGS")
						break;
				case ERROR_INVALID_PARAMETER:
					MR_LOG(LogBuildSystemUtils, Error, "MultiByteToWideChar returned: ERROR_INVALID_PARAMETER")
						break;
				case ERROR_NO_UNICODE_TRANSLATION:
					MR_LOG(LogBuildSystemUtils, Error, "MultiByteToWideChar returned: ERROR_NO_UNICODE_TRANSLATION")
						break;

				default:
					MR_LOG(LogBuildSystemUtils, Error, "MultiByteToWideChar returned: Unknown")
						break;
			}

			return;
		}

		if (PathIsRelativeW(buffer))
		{
			wchar_t exeDir[MAX_PATH] = {};
			if (!GetModuleFileNameW(nullptr,exeDir, MAX_PATH))
			{
				MR_LOG(LogBuildSystemUtils, Error, "GetModuleFileNameW returned: %s", *Utils::GetLastError());
				return;
			}

			DWORD length = wcslen(exeDir);
			PathCchRemoveFileSpec(exeDir, length);

			wchar_t* tempBuffer = buffer;

			PWSTR combinedPathNonCanonicalized;
			PathAllocCombine(exeDir, buffer, PATHCCH_ALLOW_LONG_PATHS, &combinedPathNonCanonicalized);

			buffer = combinedPathNonCanonicalized;
			LocalFree(combinedPathNonCanonicalized);
		}

		PWSTR pathWithFindable = nullptr;
		PathAllocCombine(buffer, L"\\*", PATHCCH_ALLOW_LONG_PATHS, &pathWithFindable);

		WIN32_FIND_DATAW foundFile;
		HANDLE fileHandle = FindFirstFileW(pathWithFindable, &foundFile);

		LocalFree(pathWithFindable);
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (foundFile.cFileName[0] == '.')
					continue;

				switch (foundFile.dwFileAttributes)
				{
				case FILE_ATTRIBUTE_DIRECTORY:
					{
						String nextDirectory = String::Format("%s\\%ls", name->Chr(), foundFile.cFileName);
						ListDirectory(&nextDirectory, container);
						break;
					};

					case FILE_ATTRIBUTE_ARCHIVE:
					{
						container.Add(String::Format("%s\\%ls", **name, foundFile.cFileName));
						break;
					}
				}

			} while (FindNextFileW(fileHandle, &foundFile));
		
			FindClose(fileHandle);
		}
	}
}

