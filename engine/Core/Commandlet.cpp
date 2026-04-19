/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#define _CRT_NON_CONFORMING_WCSTOK
#include "Commandlet.h"
#include <Types/String.h>
#include <Types/Array.h>
#include <Platform/Platform.h>

#include <Platform/Winapi.h>
#include <processenv.h>
#include <shellapi.h>

#include <Memory/MemoryHandler.h>

#pragma warning(disable : 6031)


#ifdef MR_PLATFORM_WINDOWS
void Commandlet::Initialize(int argumentCount, wchar_t argumentList[])
#else
void Commandlet::Initialize(int argumentCount, char argumentList[])
#endif // MR_PLATFORM_WINDOWS
{

#ifdef MR_PLATFORM_WINDOWS
	this->argumentList = CommandLineToArgvW(GetCommandLineW(), &argumentCount);
	count = argumentCount;
#endif // MR_PLATFORM_WINDOWS


	bIsInited = true;
}

void Commandlet::Shutdown()
{
#ifdef MR_PLATFORM_WINDOWS
	LocalFree(argumentList);
#endif // MR_PLATFORM_WINDOWS

	bIsInited = false;
}

String Commandlet::Parse(const char* inParam)
{
#ifdef MR_PLATFORM_WINDOWS
	wchar_t fixBufferForParameter[128] = { L'\0' };
	Platform::ConvertToWide(fixBufferForParameter, strlen(inParam), inParam);

	bool bFound = false;
	for (wchar_t** i = argumentList; *i; i++)
	{
		if (bFound)
		{
			char fixBufferForOutputParameter[128] = {};
			Platform::ConvertToNarrow(fixBufferForOutputParameter, wcslen(*i), *i);

			return fixBufferForOutputParameter;
		}
		else if (wcscmp(fixBufferForParameter, *i) == 0)
		{
			bFound = true;
		}
	}
#endif // MR_PLATFORM_WINDOWS

	return "";
}

bool Commandlet::Check(const String& inParam)
{
#ifdef MR_PLATFORM_WINDOWS
	wchar_t fixBufferForParameter[128] = { L'\0' };
	Platform::ConvertToWide(fixBufferForParameter, 128, inParam);

	wchar_t* found = wcsstr(*argumentList, fixBufferForParameter);
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
