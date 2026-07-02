/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include <HAL/Commandline.h>
#include <Logging/Log.h>
#include <Application/Application.h>

#include <Win32/MinimalWin.h>
#include <shellapi.h>

void Commandline::Init(int argc, char** argv)
{
	count = (u32)argc;
	list = argv;
}

bool Commandline::Check(const StringView& command)
{
	MR_ASSERT(count > 0 && list != nullptr, "Command check happened too early");

	for (u32 i = 0; i < count; i++)
	{
		if (strncmp(list[i], command.ptr, command.size) == 0)
			return true;
	}

	return false;
}

StringView Commandline::Get(const StringView& command)
{
	MR_ASSERT(count > 0 && list != nullptr, "Command check happened too early!");

	if (/*GetApplication()->GetCurrentState() == Application::State::PreStartup*/ true)
	{
		for (u32 i = 0; i < count; i++)
		{
			const char* flagFound = list[i % count];
			const char* flagValueFound = *list[(i + 1) % count] != '-' ? list[(i + 1) % count] : "";

			const bool bflagFound = strncmp(flagFound, command.ptr, command.size) == 0;
			const bool bflagValueFound = flagValueFound != "";

			if (bflagFound && bflagValueFound)
			{
				return { list[(i + 1) % count] };
			}
			else if (bflagFound && !bflagValueFound)
			{
				MR_LOG(LogCommandline, Warn, "%s found but hasn't got any value!", flagFound);
			}
		}
	}
	else
	{

	}

	return { "" };
}

#endif // MR_PLATFORM_WINDOWS