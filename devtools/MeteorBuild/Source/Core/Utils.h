/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include <Types/Array.h>
#include <Types/String.h>

struct Utils
{
	static void ListDirectory(String* name, Array<wchar_t*>& container);

	static void ListDirectory(String* name, Array<char*>& container);

	static String GetLastError();
};