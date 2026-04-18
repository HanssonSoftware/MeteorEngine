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

		const String sourceDirectory = Commandlet::Get().Parse("-source");
		const String intermediateDirectory = Commandlet::Get().Parse("-intermediate");

		static MemoryBlockArena<wchar_t> arena = { 32 * 1024 * 1024 };

		if (sourceDirectory && intermediateDirectory)
		{

		}

		QueryPerformanceCounter(&endTime);
		QueryPerformanceFrequency(&frequency);

		MR_LOG(LogCommands, Log, "Build command ran in %.2f seconds!", (endTime.QuadPart - startTime.QuadPart) / (frequency.QuadPart / 100.0));
	}
}