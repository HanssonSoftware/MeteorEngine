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
	void DirectorySearch(wchar_t* inPath, u32& inCount, MemoryBlockArena*& currentArena, Array<wchar_t*>& foundFiles)
	{
		wchar_t* pushedInPath = (wchar_t*)currentArena->Allocate((inCount + 8) * sizeof(wchar_t));
		wmemcpy(pushedInPath, inPath, inCount);

		wcscat(pushedInPath, L"\\*");
		u32 pushedInSize = wcslen(pushedInPath);

		WIN32_FIND_DATAW found = {};
		HANDLE inProgressSearchHandle = FindFirstFileExW(pushedInPath, FindExInfoBasic, &found, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);

		pushedInPath[pushedInSize-1] = L'\0';
		pushedInSize--;

		if (inProgressSearchHandle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (found.cFileName[0] == L'.')
					continue;
				
				if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					u32 pushedAgainSize = pushedInSize + wcslen(found.cFileName) +5;
					wchar_t* pushedAgainPath = (wchar_t*)currentArena->Allocate(pushedAgainSize * sizeof(wchar_t));

					HRESULT C = PathCchCombine(pushedAgainPath, pushedAgainSize, pushedInPath, found.cFileName);

					DirectorySearch(pushedAgainPath, pushedAgainSize, currentArena, foundFiles);
				}
				else if (found.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
				{
					u32 pushedAgainSize = pushedInSize + wcslen(found.cFileName) + 2;
					wchar_t* pushedAgainPath = (wchar_t*)currentArena->Allocate(pushedAgainSize * sizeof(wchar_t));

					HRESULT C = PathCchCombine(pushedAgainPath, pushedAgainSize, pushedInPath, found.cFileName);

					MR_LOG(LogFactory, Log, "Found file: %s", pushedAgainPath);
					foundFiles.Add(pushedAgainPath);
				}

			} while (FindNextFileW(inProgressSearchHandle, &found));

			FindClose(inProgressSearchHandle);
		}

		int J = 53;
	}


	Module MakeModuleFromBuffer(const char* buffer)
	{
		Module instance;

		const StringView header = Processing::GetWord(buffer);
		if (!strncmp((char*)header.ptr, "Module", header.size))
		{
			const StringView moduleName = Processing::GetQuotedWord(buffer);
			instance.SetModuleName(moduleName);

			if (Processing::ExpectedCharacter(buffer, ':'))
			{
				buffer++; // :

				const StringView parent = Processing::GetQuotedWord(buffer);
				instance.SetParent(parent);

				buffer++;
				if (Processing::ExpectedCharacter(buffer, '{'))
				{
					buffer++;

					const char* last = buffer;
					while (*buffer)
					{
						const String command = String(Processing::GetWord(buffer));

						//constexpr u64 b = "Dependencies"_h;

						switch (Hash(command))
						{
						case 5786862590791793456:  // IncludePath
						case 11138787046201537241: // IncludePaths
							Processing::EnterBlock(buffer, command, &instance);
							break;

						case 14686587794296974560: // Dependencies 
							Processing::EnterBlock(buffer, command, &instance);
							break;

						default:
							Processing::SkipBlock(buffer);
							break;
						}

						if (last == buffer)
						{
							//MR_LOG(LogProcessing, Error, "Parser stuck!");
							break;
						}
						else
						{
							last = buffer;
						}
					}
				}
			}
			else
			{
				//MR_LOG(LogProcessing, Error, "Module has no parent! %s", *instance.moduleName);
			}
		}
		//else
		{
			//MR_LOG(LogProcessing, Fatal, "Failed to parse script header word! %s", header.ptr);
		}

		return instance;
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

