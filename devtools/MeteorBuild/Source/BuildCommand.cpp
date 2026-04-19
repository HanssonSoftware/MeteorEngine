/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Commands.h"
#include <Logging/Log.h>
#include <Commandlet.h>
#include <Memory/MemoryBlockArena.h>

#include "CommandRegistry.h"

#include <Platform/Winapi.h>

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
			DirectorySearch(sourceDirectoryW, files);
		}

		QueryPerformanceCounter(&endTime);
		QueryPerformanceFrequency(&frequency);

		MR_LOG(LogCommands, Log, "Build command ran in %.2f seconds!", (endTime.QuadPart - startTime.QuadPart) / (frequency.QuadPart / 100.0));
	}
}