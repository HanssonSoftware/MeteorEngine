/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "Commands.h"
#include <HAL/Commandline.h>

#include "Win32/MinimalWin.h"
#include <shlwapi.h>
#include <pathcch.h>

//#include <strsafe.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

LOG_ADDCATEGORY(Factory);

namespace Commands
{
	void DirectorySearch(wchar_t* inPath, u32& inCount, Array<wchar_t*>& foundFiles)
	{
		wchar_t* pushedInPath = inPath + inCount + 2;

		HRESULT A = PathCchCombine(pushedInPath, inCount, inPath, L"*");
		u32 pushedInSize = wcslen(pushedInPath);

		WIN32_FIND_DATAW found = {};
		HANDLE inProgressSearchHandle = FindFirstFileW(pushedInPath, &found);

		PathCchRemoveFileSpec(pushedInPath, pushedInSize);
		pushedInSize = wcslen(pushedInPath);

		if (inProgressSearchHandle != INVALID_HANDLE_VALUE)
		{
			do
			{

			} while (FindNextFileW(inProgressSearchHandle, &found));

			FindClose(inProgressSearchHandle);
		}

		int J = 53;
	}

		//

		//MR_ASSERT(directory != nullptr, "Directory cannot be invalid!");

		//if (PathIsRelativeW(directory))
		//{
		//	wchar_t exeLocation[MAX_PATH] = {};

		//	wcscpy_s(exeLocation, MAX_PATH, Commandlet::Get().GetArgs()[0]);
		//	PathRemoveFileSpecW(exeLocation);

		//	const u32 combinedDirectorySize = (u32)wcslen(exeLocation) + (u32)wcslen(directory);
		//	wchar_t* newDirectory = (wchar_t*)arena->Allocate((combinedDirectorySize + 10) * sizeof(wchar_t));

		//	PathCchCombine(newDirectory, combinedDirectorySize, exeLocation, directory);
		//	directory = newDirectory;
		//}

		//if (*PathFindExtensionW(directory))
		//	PathCchRemoveFileSpec(directory, wcslen(directory));

		//HRCHECK(PathCchCombine(directory, wcslen(directory) + 5, directory, L"*"));

		//WIN32_FIND_DATAW foundFile = {};
		//HANDLE fileHandle = FindFirstFileExW(directory, FindExInfoBasic, &foundFile, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);

		//HRCHECK(PathCchRemoveFileSpec(directory, wcslen(directory)));

		//MR_ASSERT(fileHandle != INVALID_HANDLE_VALUE, "Unable to list path at %ls", directory);

		//do
		//{
		//	if (foundFile.cFileName[0] == L'.')
		//		continue;

		//	if (foundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		//	{
		//		const u32 nextSearchableDirectorySize = (u32)swprintf(nullptr, 0, L"%ls\\%ls", directory, foundFile.cFileName) + 1;
		//		wchar_t* newDirectoryName = (wchar_t*)arena->Allocate(nextSearchableDirectorySize * sizeof(wchar_t));

		//		if (swprintf(newDirectoryName, nextSearchableDirectorySize, L"%ls\\%ls", directory, foundFile.cFileName))
		//		{
		//			DirectorySearch(newDirectoryName, foundFiles, arena);
		//		}
		//	}
		//	else if (foundFile.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		//	{
		//		const u32 nextSearchableDirectorySize = (u32)swprintf(nullptr, 0, L"%ls\\%ls", directory, foundFile.cFileName) + 1;
		//		wchar_t* newDirectoryName = (wchar_t*)arena->Allocate(nextSearchableDirectorySize * sizeof(wchar_t));

		//		if (swprintf(newDirectoryName, nextSearchableDirectorySize, L"%ls\\%ls", directory, foundFile.cFileName))
		//		{
		//			foundFiles.Add(newDirectoryName);
		//		}
		//	}

		//} while (FindNextFileW(fileHandle, &foundFile));

		//FindClose(fileHandle);
}

#pragma warning(disable : 6387)


//String Utils::GetError()
//{
//	wchar_t fixed[512] = {};
//
//	const u32 returned = (u32)FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, ::GetLastError(), LANG_USER_DEFAULT, fixed, 511, nullptr);
//	fixed[returned - 2] = L'\0';
//
//	char fixedTwo[512] = {};
//	WideCharToMultiByte(CP_UTF8, 0, fixed, returned, fixedTwo, returned, nullptr, nullptr);
//
//	return fixedTwo;
//}
//
//void Utils::ListDirectory(char* name, Array<char*>& container)
//{
//	MR_ASSERT(false, "Not implemented!");
//}
//
//void Utils::ListDirectory(wchar_t* name, Array<wchar_t*>& container)
//{
//	if (name != nullptr)
//	{
//		wchar_t fixed[512 + 1] = {};
//		const u32 lengthOfDirectoryName = (u32)wcslen(name);
//
//		wchar_t* charBuffer = lengthOfDirectoryName > 512 ? (wchar_t*)GetMemoryManager()->Allocate((lengthOfDirectoryName + (lengthOfDirectoryName % 30)) * sizeof(wchar_t)) : fixed;
//
//		if (PathIsRelativeW(name))
//		{
//			wchar_t exeDir[512 + 1] = { L'\0' };
//			if (const DWORD moduleFileName = GetModuleFileNameW(nullptr, exeDir, 512))
//			{
//				PathCchRemoveFileSpec(exeDir, moduleFileName);
//
//				if (FAILED(PathCchCombine(charBuffer, 512, exeDir, name)))
//				{
//
//				}
//
//				//PathCchCanonicalize(charBuffer, 512, charBuffer);
//			}
//
//		}
//		else
//		{
//			wcsncpy(charBuffer, name, lengthOfDirectoryName);
//		}
//
//		wcscat(charBuffer, L"\\*");
//
//		WIN32_FIND_DATAW foundFile;
//		HANDLE fileHandle = FindFirstFileExW(charBuffer, FindExInfoBasic, &foundFile, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
//
//		PathCchRemoveFileSpec(charBuffer, wcslen(charBuffer));
//		if (fileHandle != INVALID_HANDLE_VALUE)
//		{
//			do
//			{
//				if (foundFile.cFileName[0] == L'.')
//					continue;
//
//				if (foundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//				{
//					static MemoryBlockArena<wchar_t> directoryArena = { 16 * 1024 * 1024 };
//
//					const u32 nextSearchableDirectorySize = (u32)swprintf(nullptr, 0, L"%ls\\%ls", charBuffer, foundFile.cFileName) + 1;
//					wchar_t* newDirectoryName = (wchar_t*)directoryArena.Allocate(nextSearchableDirectorySize * sizeof(wchar_t));
//
//					if (swprintf(newDirectoryName, nextSearchableDirectorySize, L"%ls\\%ls", charBuffer, foundFile.cFileName))
//					{
//						ListDirectory(newDirectoryName, container);
//					}
//				}
//				else if (foundFile.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
//				{
//					static MemoryBlockArena<wchar_t> fileArena = { 16 * 1024 * 1024 };
//
//					const u32 nextSearchableDirectorySize = (u32)swprintf(nullptr, 0, L"%ls\\%ls", charBuffer, foundFile.cFileName) + 1;
//					wchar_t* newDirectoryName = (wchar_t*)fileArena.Allocate(nextSearchableDirectorySize * sizeof(wchar_t));
//
//					if (swprintf(newDirectoryName, nextSearchableDirectorySize, L"%ls\\%ls", charBuffer, foundFile.cFileName))
//					{
//						container.Add(newDirectoryName);
//					}
//				}
//
//			} while (FindNextFileW(fileHandle, &foundFile));
//
//			FindClose(fileHandle);
//		}
//
//		if (lengthOfDirectoryName > 512) GetMemoryManager()->Deallocate(charBuffer, (lengthOfDirectoryName + (lengthOfDirectoryName % 30)) * sizeof(wchar_t));
//	}
//}

