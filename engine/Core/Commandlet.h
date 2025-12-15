/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Logging/Log.h>
#include <Types/Array.h>
#include <Types/String.h>

class String;

LOG_ADDCATEGORY(Commandlet);

struct Commandlet
{
	static void Initialize();

	static bool Parse(const String& inParam, String* returnVal);

protected:
	static inline bool bIsInited = false;

	static inline Array<String> parsedWords;
};