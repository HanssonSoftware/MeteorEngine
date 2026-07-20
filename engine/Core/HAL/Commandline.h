/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Logging/Log.h>
#include <Types/StringView.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

LOG_ADDCATEGORY(Commandline);

struct CORE_API Commandline
{
	void Init(int argc, char** argv);

	bool Check(const StringView& command) const;

	StringView Get(const StringView& command) const;

	const char* const* GetInternal() const { return list; };
protected:
	char** list = nullptr;

	u32 count = 0;

	//bool bIsApplicationInWarmup = true;
};