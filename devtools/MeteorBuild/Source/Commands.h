/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Types/String.h>

#ifdef MR_PLATFORM_WINDOWS
#include <errhandlingapi.h>

LOG_ADDCATEGORY(HResultChecks);

#define HRCHECK_UNDER(x, fate) \
	if (FAILED(x)) \
	{			   \
		MR_LOG(LogHResultChecks, fate, "HRESULT failed with: (%d)  %s", (u32)::GetLastError(), *Commands::GetLastErrorString()); \
	}


#define HRCHECK(x) HRCHECK_UNDER(x, Fatal)
#else
#define HRCHECK() // This macro does nothing, besides decoration.
#endif // MR_PLATFORM_WINDOWS

namespace Commands
{
	void Build_Cmd();

#ifdef MR_PLATFORM_WINDOWS
	void DirectorySearch(wchar_t* directory, Array<wchar_t*>& foundFiles);

	String GetLastErrorString();
#else
	void DirectorySearch(char* directory, Array<char*>& foundFiles);
#endif // MR_PLATFORM_WINDOWS
}