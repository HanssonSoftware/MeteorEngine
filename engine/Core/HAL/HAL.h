/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <HAL/DataTypes.h>
#include <Types/StringView.h>

namespace HAL
{
	bool ConvertToWide(wchar_t* targetBuffer, const u32 size, const char* convertibleBuffer);

	bool ConvertToNarrow(char* targetBuffer, const u32 size, const wchar_t* convertibleBuffer);

	StringView LocalizeErrorCode(i64 code);
};