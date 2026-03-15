/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */


#include "Utils.h"
#include <Types/String.h>

#include <Memory/MemoryBlockArena.h>

#include <Platform/Winapi.h>
#include <pathcch.h>

#include <shlwapi.h>
#include <strsafe.h>
#include <Platform.h>
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

#pragma warning(disable : 6387)

LOG_ADDCATEGORY(BuildSystemUtils);

String Utils::GetError()
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

void Utils::ListDirectory(char* name, Array<char*>& container)
{
	MR_ASSERT(false, "Not implemented!");
}

void Utils::ListDirectory(wchar_t* name, Array<wchar_t*>& container)
{
	if (name != nullptr)
	{
		wchar_t fixed[512 + 1] = {};
		const u32 lengthOfDirectoryName = (u32)wcslen(name);

		wchar_t* charBuffer = lengthOfDirectoryName > 512 ? (wchar_t*)GetMemoryManager()->Allocate((lengthOfDirectoryName + (lengthOfDirectoryName % 30)) * sizeof(wchar_t)) : fixed;

		if (PathIsRelativeW(name))
		{
			wchar_t exeDir[512 + 1] = { L'\0' };
			if (const DWORD moduleFileName = GetModuleFileNameW(nullptr, exeDir, 512))
			{
				PathCchRemoveFileSpec(exeDir, moduleFileName);
				
				if (FAILED(PathCchCombine(charBuffer, 512, exeDir, name)))
				{

				}

				//PathCchCanonicalize(charBuffer, 512, charBuffer);
			}

		}
		else
		{
			wcsncpy(charBuffer, name, lengthOfDirectoryName);
		}

		wcscat(charBuffer, L"\\*");

		WIN32_FIND_DATAW foundFile;
		HANDLE fileHandle = FindFirstFileExW(charBuffer, FindExInfoBasic, &foundFile, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);

		PathCchRemoveFileSpec(charBuffer, wcslen(charBuffer));
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (foundFile.cFileName[0] == L'.')
					continue;

				if (foundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					static MemoryBlockArena<wchar_t> directoryArena = { 16 * 1024 * 1024 };

					const u32 nextSearchableDirectorySize = (u32)swprintf(nullptr, 0, L"%ls\\%ls", charBuffer, foundFile.cFileName) + 1;
					wchar_t* newDirectoryName = (wchar_t*)directoryArena.Allocate(nextSearchableDirectorySize * sizeof(wchar_t));

					if (swprintf(newDirectoryName, nextSearchableDirectorySize, L"%ls\\%ls", charBuffer, foundFile.cFileName))
					{
						ListDirectory(newDirectoryName, container);
					}
				}
				else if (foundFile.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
				{
					static MemoryBlockArena<wchar_t> fileArena = { 16 * 1024 * 1024 };

					const u32 nextSearchableDirectorySize = (u32)swprintf(nullptr, 0, L"%ls\\%ls", charBuffer, foundFile.cFileName) + 1;
					wchar_t* newDirectoryName = (wchar_t*)fileArena.Allocate(nextSearchableDirectorySize * sizeof(wchar_t));

					if (swprintf(newDirectoryName, nextSearchableDirectorySize, L"%ls\\%ls", charBuffer, foundFile.cFileName))
					{
						container.Add(newDirectoryName);
					}
				}

			} while (FindNextFileW(fileHandle, &foundFile));
		
			FindClose(fileHandle);
		}

		if (lengthOfDirectoryName > 512) GetMemoryManager()->Deallocate(charBuffer, (lengthOfDirectoryName + (lengthOfDirectoryName % 30)) * sizeof(wchar_t));
	}
}

