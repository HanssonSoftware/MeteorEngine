/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <HAL/DataTypes.h>

class String;

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

namespace HAL
{
	CORE_API bool ConvertToWide(wchar_t* targetBuffer, const u32 size, const char* convertibleBuffer);

	CORE_API bool ConvertToNarrow(char* targetBuffer, const u32 size, const wchar_t* convertibleBuffer);

	void* LocalizeErrorCode(i64 code);

	bool PeekOSMessageQueue();

	void InitHAL();

	void ShutdownEssential();

	CORE_API u32 FatalExit(u32 code);

	CORE_API void* GetEngineCore();

#ifdef MR_DEBUG
	void DebugBreak();
#endif // MR_DEBUG
};