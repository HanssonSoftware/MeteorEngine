/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Commands.h"
#include <Logging/Log.h>
#include <Commandlet.h>
#include <Memory/MemoryBlockArena.h>

#include "CommandRegistry.h"

#include <Platform/Winapi.h>
#include <Shlwapi.h>

LOG_ADDCATEGORY(Commands);

namespace Commands
{
	ADD_NEW_BUILD_COMMAND("-build", "Builds your project", Build_Cmd);
	
	void Build_Cmd()
	{
		LARGE_INTEGER startTime, endTime, frequency;
		QueryPerformanceCounter(&startTime);

		const String sourceDirectory = Commandlet::Get().Parse("-s");
		const String intermediateDirectory = Commandlet::Get().Parse("-i");

		// Arena allocates bytes, so equal size will be allocated!
		// WChar: 32 MB (Char / 2)
		// Char: 32 MB

#ifdef MR_PLATFORM_WINDOWS
		static MemoryBlockArena<wchar_t> arena = { 32 * 1024 * 1024 };
#else
		static MemoryBlockArena<char> arena = { 32 * 1024 * 1024 };
#endif // MR_PLATFORM_WINDOWS

		if (sourceDirectory && intermediateDirectory)
		{
			wchar_t* sourceDirectoryW = (wchar_t*)arena.Allocate(sourceDirectory.Length() * sizeof(wchar_t));
			if (!MultiByteToWideChar(CP_UTF8, 0, sourceDirectory, sourceDirectory.Length(), sourceDirectoryW, sourceDirectory.Length()))
			{
				MR_LOG(LogCommands, Error, "%s", *GetLastErrorString());
				return;
			}

			Array<wchar_t*> files;
			DirectorySearch(sourceDirectoryW, files, &arena);

			for (auto& file : files)
			{
				u64 h = ".cpp"_h;
				u64 hs = ".h"_h;

				wchar_t* end = PathFindExtensionW(file);
				switch (Hash(reinterpret_cast<char*>(end)))
				{
				case 12638124528392833969: // .cpp
				{

					int J = 43;
					break;
				}
				case 565857095260348859: // .h
				{

					int J = 43;
					break;
				}
				default:
					break;
				}

				int J = 43;
			}
		}

		QueryPerformanceCounter(&endTime);
		QueryPerformanceFrequency(&frequency);

		MR_LOG(LogCommands, Log, "Build command ran in %.2f seconds!", (endTime.QuadPart - startTime.QuadPart) / (frequency.QuadPart / 100.0));
	}
}