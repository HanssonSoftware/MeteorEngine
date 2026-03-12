/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Utils.h"
#include <Types/String.h>

#include <Platform/Winapi.h>
#include <PathCch.h>

#include <shlwapi.h>
#include <strsafe.h>
#include <Platform.h>
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

void Utils::ListDirectory(String* name, Array<char*>& container)
{
	MR_ASSERT(false, "Not implemented!");
}

void Utils::ListDirectory(String* name, Array<wchar_t*>& container)
{
	if (name != nullptr)
	{
		wchar_t fixed[256 + 1] = {};
		wchar_t* charBuffer = name->Length() > 256 ? (wchar_t*)GetMemoryManager()->Allocate(name->Length() * sizeof(wchar_t)) : fixed;
		Platform::ConvertToWide(charBuffer, name->Length(), **name);

		if (PathIsRelativeW(charBuffer))
		{
			wchar_t exeDir[512] = {};
			if (DWORD moduleFileName = GetModuleFileNameW(nullptr, exeDir, 512))
			{
				PathCchRemoveFileSpec(exeDir, moduleFileName);
				int J = 75;
				
			}

		}

		wcscat(charBuffer, L"\\*");

		WIN32_FIND_DATAW foundFile;
		HANDLE fileHandle = FindFirstFileW(charBuffer, &foundFile);

		PathCchRemoveFileSpec(charBuffer, wcslen(charBuffer));
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (foundFile.cFileName[0] == L'.')
					continue;

				if (foundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					//wchar_t* tempName = new wchar_t[wcslen(charBuffer) + wcslen(foundFile.cFileName) + 10]();
					//snprintf("");
					//ListDirectory(tempName, container);
				}
				else if (foundFile.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
				{
					wchar_t* tempName = new wchar_t[wcslen(charBuffer) + wcslen(foundFile.cFileName) + 10]();
					StringCchPrintfW(tempName, STRSAFE_MAX_CCH, L"%ls\\%ls", charBuffer, foundFile.cFileName);
					//container.Add(tempName);
				
					delete[] tempName;
				}

			} while (FindNextFileW(fileHandle, &foundFile));
		
			FindClose(fileHandle);
		}
	}
}

