/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include <HAL/File.h>
#include <Logging/Log.h>
#include <HAL/HAL.h>

#include "MinimalWin.h"

LOG_ADDCATEGORY(HAL);

static constexpr DWORD CombineAccess(Access type)
{
	DWORD num = 0;

	if (type & Access::Read)
	{
		num |= GENERIC_READ;
	}
	else if (type == Access::Write)
	{
		num |= GENERIC_WRITE;
	}
	else if (type == Access::None)
	{
		num |= 0;
	}

	return num;
}

static constexpr DWORD CombineShare(Share type)
{
	DWORD num = 0;

	if (type & Share::Reading)
	{
		num |= FILE_SHARE_READ;
	}
	else if (type == Share::Writing)
	{
		num |= FILE_SHARE_WRITE;
	}
	else if (type == Share::Delete)
	{
		num |= FILE_SHARE_DELETE;
	}
	else if (type == Share::Nothing)
	{
		num |= 0;
	}

	return num;
}

static constexpr DWORD TranslateToWin(OpenAs openAs)
{
	switch (openAs)
	{
	case AlwaysCreate:
		return 2;
	case CreateIfNotExist:
		return 1;
	case JustOpen:
		return 4;
	case OpenIfExist:
		return 3;
	case Truncate:
		return 5;
	}

	return -1;
}

bool File::Open(const StringView& name, const OpenAs openAs,const Access access, const Share share)
{
	if (!name.ptr)
	{
		MR_LOG(LogHAL, Warn, "The filename is empty!");
		return false;
	}

	wchar_t path[2048] = {};
	if (!HAL::ConvertToWide(path, name.size, (char*)name.ptr))
	{
		MR_LOG(LogHAL, Error, "Conversion error occoured!");
		return false;
	}

	const DWORD accessMode = CombineAccess(access);
	const DWORD shareMode = CombineShare(share);

	windowsAPIHandle = CreateFileW(path, accessMode, shareMode, nullptr, TranslateToWin(openAs), FILE_ATTRIBUTE_NORMAL, nullptr);
	if (windowsAPIHandle == INVALID_HANDLE_VALUE)
	{
		//MR_LOG(LogHAL, );
	}

	LARGE_INTEGER lg;
	GetFileSizeEx(windowsAPIHandle, &lg);

	fileSize = (u64)lg.QuadPart;

	return true;
}

void File::Read(char* big)
{
	DWORD read = 0;
	if (!ReadFile(windowsAPIHandle, big, (DWORD)fileSize, &read, nullptr))
	{
	
	}
}

#endif // MR_PLATFORM_WINDOWS












// Template below

/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS

#endif // MR_PLATFORM_WINDOWS