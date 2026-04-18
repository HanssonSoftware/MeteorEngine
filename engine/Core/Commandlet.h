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

struct CORE_API Commandlet
{
	static Commandlet& Get()
	{
		static Commandlet instance;
		return instance;
	}

#ifdef MR_PLATFORM_WINDOWS
	void Initialize(int argumentCount, wchar_t argumentList[]);
#else
	void Initialize(int argumentCount, char argumentList[]);
#endif // MR_PLATFORM_WINDOWS


	void Shutdown();

	String Parse(const char* inParam);

	bool Check(const String& inParam);

	u32 GetCount() const { return count; };

#ifdef MR_PLATFORM_WINDOWS
	const wchar_t* const* GetArgs() { return (const wchar_t* const*)argumentList; };
#else
	const char* const* GetArgs() { return (const char* const*)argumentList; };
#endif // MR_PLATFORM_WINDOWS

protected:
#ifdef MR_PLATFORM_WINDOWS
	wchar_t** argumentList;
#else
	char** argumentList;
#endif // MR_PLATFORM_WINDOWS
	
	u32 count = 0;

	bool bIsInited = false;
};