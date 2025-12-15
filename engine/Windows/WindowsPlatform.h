/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/Platform.h>

struct WindowsPlatform : public IPlatform
{
	static ScopedPtr<wchar_t> ConvertToWide(const char* skinny);

	static ScopedPtr<char> ConvertToNarrow(const wchar_t* fat);

	static String GetError();

	static bool IsRunningAnAnotherInstance();
};


//using Platform = WindowsPlatform;
