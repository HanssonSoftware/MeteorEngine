/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Platform.h"
#include <Logging/Log.h>
#include <Windows/Windows.h>
#include <stringapiset.h>

LOG_ADDCATEGORY(Platform);

void Platform::ConvertToWide(wchar_t* targetBuffer, const u32 size, const char* convertibleBuffer)
{
	if (size && targetBuffer && convertibleBuffer)
	{
		MultiByteToWideChar(CP_UTF8, 0, convertibleBuffer, size, targetBuffer, size);
		return;
	}

	MR_LOG(LogPlatform, Error, "Unable to convert narrow buffer to wide!");
}

void Platform::ConvertToNarrow(char* targetBuffer, const u32 size, const wchar_t* convertibleBuffer)
{
	if (size && targetBuffer && convertibleBuffer)
	{
		WideCharToMultiByte(CP_UTF8, 0, convertibleBuffer, size, targetBuffer, size, nullptr, nullptr);
		return;
	}

	MR_LOG(LogPlatform, Error, "Unable to convert wide buffer to narrow!");
}
