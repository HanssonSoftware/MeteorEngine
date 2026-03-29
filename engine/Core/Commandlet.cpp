/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#define _CRT_NON_CONFORMING_WCSTOK
#include "Commandlet.h"
#include <Types/String.h>
#include <Types/Array.h>
#include <Platform/Platform.h>

#include <Platform/Winapi.h>
#include <processenv.h>

#include <Memory/MemoryHandler.h>

#pragma warning(disable : 6031)


void Commandlet::Initialize(int argumentCount, char argumentList[])
{
	bIsInited = true;
}

void Commandlet::Shutdown()
{
	bIsInited = false;
}

String Commandlet::Parse(const char* inParam)
{
#ifdef MR_PLATFORM_WINDOWS
	wchar_t* wideCommandLine = GetCommandLineW();

	wchar_t fixBufferForParameter[128] = { L'\0' };
	Platform::ConvertToWide(fixBufferForParameter, 128, inParam);

	wchar_t* found = wcsstr(wideCommandLine, fixBufferForParameter);
	if (found)
	{
		wchar_t* end = found;

		// to first space
		while (*end && !isspace(*end))
			end++;

		end++;
		found = end;

		// to second word
		end++;
		if (*end != L'-')
		{
			end++;

			while (*end && !isspace(*end))
				end++;

			char fixBufferForOutputParameter[128] = { '\0' };
			Platform::ConvertToNarrow(fixBufferForOutputParameter, (u32)(end - found), found);

			return String(fixBufferForOutputParameter, (u32)(end - found));
		}

		return "";
	}
#endif // MR_PLATFORM_WINDOWS


	return "";
}

bool Commandlet::Check(const String& inParam)
{
#ifdef MR_PLATFORM_WINDOWS
	wchar_t* wideCommandLine = GetCommandLineW();

	wchar_t fixBufferForParameter[128] = { L'\0' };
	Platform::ConvertToWide(fixBufferForParameter, 128, inParam);

	wchar_t* found = wcsstr(wideCommandLine, fixBufferForParameter);
	if (found)
	{
		wchar_t* end = found;

		// to first space
		while (*end && !isspace(*end))
			end++;

		return wcsncmp(found, fixBufferForParameter, end - found) == 0;
	}
#endif // MR_PLATFORM_WINDOWS

	return false;
}
