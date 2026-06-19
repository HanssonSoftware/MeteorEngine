/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include <HAL/File.h>
#include <Logging/Log.h>

#include "WinMin.h"

LOG_ADDCATEGORY(HAL);

bool File::Open(const char* name, const Access access, const Share share)
{
	if (!name)
	{
		MR_LOG(LogHAL, Warn, "The filename is empty!");
		return false;
	}

	//wchar_t* 
	//MultiByteToWideChar(CP_UTF8, 0, name, strlen(name), );

	return false;
}

#endif // MR_PLATFORM_WINDOWS












// Template below

/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS

#endif // MR_PLATFORM_WINDOWS