/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/DataTypes.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

struct CORE_API Platform
{
	static void ConvertToWide(wchar_t* targetBuffer, const u32 size, const char* convertibleBuffer);

	static void ConvertToNarrow(char* targetBuffer, const u32 size, const wchar_t* convertibleBuffer);
};