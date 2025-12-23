/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Logging/Log.h>
#include <Types/Array.h>
#include <Types/String.h>
// #include <CoreProxy.h>
class String;

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

LOG_ADDCATEGORY(Commandlet);

struct CORE_API Commandlet
{
	static void Initialize();

	static bool Parse(const String& inParam, String* returnVal);

protected:
	static inline bool bIsInited = false;
};