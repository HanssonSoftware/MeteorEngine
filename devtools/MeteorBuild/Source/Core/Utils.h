/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include <Types/Array.h>
#include <Types/String.h>

struct Utils
{
	static void ListDirectory(wchar_t* name, Array<StringW>& container);

	static String GetLastError();
};