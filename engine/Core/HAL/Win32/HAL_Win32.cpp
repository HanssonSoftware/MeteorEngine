/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS

#include <HAL/HAL.h>
#include <Logging/Log.h>
#include "MinimalWin.h"
#include <stringapiset.h>

LOG_ADDCATEGORY(HAL);

bool HAL::ConvertToWide(wchar_t* targetBuffer, const u32 size, const char* convertibleBuffer)
{
	if (size && targetBuffer && convertibleBuffer)
	{
		MultiByteToWideChar(CP_UTF8, 0, convertibleBuffer, size, targetBuffer, size);
		return true;
	}

	MR_LOG(LogHAL, Error, "Conversion error! MultiByteToWideChar[%d:%s]", GetLastError(), LocalizeErrorCode(GetLastError()));
	return false;
}

bool HAL::ConvertToNarrow(char* targetBuffer, const u32 size, const wchar_t* convertibleBuffer)
{
	if (size && targetBuffer && convertibleBuffer)
	{
		WideCharToMultiByte(CP_UTF8, 0, convertibleBuffer, size, targetBuffer, size, nullptr, nullptr);
		return true;
	}

	MR_LOG(LogHAL, Error, "Conversion error! WideCharToMultiByte[%d:%s]", GetLastError(), LocalizeErrorCode(GetLastError()));
	return false;
}

StringView HAL::LocalizeErrorCode(i64 code)
{
	char final[512] = {};

	const DWORD count = FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		(DWORD)code,
		LANG_USER_DEFAULT,
		final,
		511,
		nullptr
		);

	if (count == 0)
	{
		MR_LOG(LogHAL, Error, "Conversion error! WideCharToMultiByte[%d:%s]");
		return { "" };
	}

	//char end[512] = {};
	//ConvertToNarrow(end, count - 2, final);

	return { final, count - 2 };
}

#endif // MR_PLATFORM_WINDOWS