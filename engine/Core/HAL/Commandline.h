/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Logging/Log.h>
#include <Types/String.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

LOG_ADDCATEGORY(Commandlet);

struct CORE_API Commandline
{
	void Init(int argc, char** argv);

	bool Check(const char* command);

	StringView Get(const char* command);

protected:
	char** list = nullptr;

	u32 count = 0;
};