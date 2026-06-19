/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <HAL/DataTypes.h>

namespace HAL
{
	void ConvertToWide(wchar_t* targetBuffer, const u32 size, const char* convertibleBuffer);

	void ConvertToNarrow(char* targetBuffer, const u32 size, const wchar_t* convertibleBuffer);
};