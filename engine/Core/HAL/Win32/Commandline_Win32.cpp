/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include <HAL/Commandline.h>
#include <Logging/Log.h>

#include <Win32/MinimalWin.h>
#include <shellapi.h>

void Commandline::Init(int argc, char** argv)
{
	count = (u32)argc;
	list = argv;
}

bool Commandline::Check(const char* command)
{
	MR_ASSERT(count > 0 && list != nullptr, "Early command check happened!");

	for (u32 i = 0; i < count; i++)
	{
		if (strcmp(list[i], command) == 0)
			return true;
	}

	return false;
}

StringView Commandline::Get(const char* command)
{
	MR_ASSERT(count > 0 && list != nullptr, "Early command check happened!");

	for (u32 i = 0; i < count; i++)
	{
		bool a = strcmp(list[i], command) == 0;
		bool b = *list[i++ % count] != '-';

		if (a &&b)
		{
			return { nullptr };
		}
	}

	return { nullptr };
}

#endif // MR_PLATFORM_WINDOWS