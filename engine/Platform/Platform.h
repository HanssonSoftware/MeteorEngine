/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Pointers.h>
#include <Types/String.h>

struct IPlatform
{
	static ScopedPtr<wchar_t> ConvertToWide(const char* skinny) { return ScopedPtr<wchar_t>(); };

	static ScopedPtr<char> ConvertToNarrow(const wchar_t* fat) { return ScopedPtr<char>(); };

	static String GetError() { return ""; };

	static bool IsRunningAnAnotherInstance() {};
};

typedef IPlatform Platform;
