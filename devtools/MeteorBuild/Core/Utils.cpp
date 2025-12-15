/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Utils.h"
//#include "Parser.h"
#include <Platform/FileManager.h>
#include <Platform/Paths.h>
#include <Types/Pointers.h>
#include <Platform/Platform.h>

#include "Windows/WindowsPaths.h"
#include <PathCch.h>

void Utils::ListDirectory(String* name, Array<String>& container)
{
	if (name != nullptr)
	{
		if (FileManager::IsPathRelative(name))
		{
			String exeDir = WindowsPaths::GetExecutableDirctory();

			PathCchRemoveFileSpec(exeDir.Data(), exeDir.Length());
			exeDir.Refresh();

			PWSTR combinedPathNonCanonicalized;
			PathAllocCombine(exeDir.Data(), *name, PATHCCH_ALLOW_LONG_PATHS, &combinedPathNonCanonicalized);
			exeDir.Refresh();

			*name = combinedPathNonCanonicalized;
			LocalFree(combinedPathNonCanonicalized);
		}

		WIN32_FIND_DATAW foundFile;
		HANDLE fileHandle = FindFirstFileW(String::Format("%ls\\*", name->Chr()), &foundFile);

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
						String nextDirectory = String::Format("%ls\\%ls", name->Chr(), foundFile.cFileName);
						ListDirectory(&nextDirectory, container);
						break;
					};

					case FILE_ATTRIBUTE_ARCHIVE:
					{
						container.Add(String::Format("%ls\\%ls", **name, foundFile.cFileName));
						break;
					}
				}

			} while (FindNextFileW(fileHandle, &foundFile));
		
			FindClose(fileHandle);
		}
	}
}

