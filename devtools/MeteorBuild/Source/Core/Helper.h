/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Logging/Log.h>

#ifdef MR_PLATFORM_WINDOWS
#include <errhandlingapi.h>


LOG_ADDCATEGORY(HResultChecks);

#define HRCHECK_UNDER(x, fate) \
	if (FAILED(x)) \
	{			   \
		MR_LOG(LogHResultChecks, fate, "HRESULT failed with: (%d)  %s", (u32)::GetLastError(), *Utils::GetError()); \
	}


#define HRCHECK(x) HRCHECK_UNDER(x, Fatal)
#else
#define HRCHECK() // This macro does nothing, besides decoration.
#endif // MR_PLATFORM_WINDOWS

