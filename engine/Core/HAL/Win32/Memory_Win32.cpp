/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include <HAL/Memory.h>
#include <Logging/Log.h>

#include "WinMin.h"

LOG_ADDCATEGORY(HAL);

namespace HAL
{
	void* OSAlloc(void* specificLocationToStart = nullptr, const u64 size)
	{
		return VirtualAlloc(specificLocationToStart, RoundTo(size, 2), MEM_COMMIT | MEM_RESERVE, 0);
	}

	void OSDealloc(void* location, const u64 size)
	{
		if (!location)
		{
			return;
		}

		VirtualFree(location, 0, MEM_RELEASE);
	}
}

#endif // MR_PLATFORM_WINDOWS