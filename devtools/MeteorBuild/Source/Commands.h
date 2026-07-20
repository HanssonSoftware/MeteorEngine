/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Types/String.h>
#include "Module.h"
#include <Memory/MemoryBlockArena.h>

#ifdef MR_PLATFORM_WINDOWS

LOG_ADDCATEGORY(HResultChecks);

#define HRCHECK_UNDER(x, fate) \
	if (FAILED(x)) \
	{			   \
		MR_LOG(LogHResultChecks, fate, "%s=%d", L#x,(u32)::GetLastError()); \
		return;\
	}


#define HRCHECK(x) HRCHECK_UNDER(x, Fatal)
#else
#define HRCHECK() // This macro does nothing, besides decoration.
#endif // MR_PLATFORM_WINDOWS

struct FoundUnit
{
	FoundUnit() = default;
	~FoundUnit() noexcept = default;

#ifdef MR_PLATFORM_WINDOWS
	const wchar_t* path;
#else
	const char* path;
#endif // MR_PLATFORM_WINDOWS
	enum Type
	{
		SOURCE,
		HEADER,
		NATVIS,
		BUILD_SCRIPT
	} type;

#ifdef MR_PLATFORM_WINDOWS
	FoundUnit(const wchar_t* path, Type type)
		: path(path)
		, type(type)
	{

	}
#else
	FoundUnit(const char* path, Type type)
		: path(path)
		, type(type)
	{

	}
#endif // MR_PLATFORM_WINDOWS
};


namespace Commands
{
	bool Build_Cmd();

	bool Generate_Cmd();

	void AddSolution(const Array<Module>* modules);

	Module MakeModuleFromBuffer(const char* buffer);

#ifdef MR_PLATFORM_WINDOWS
	void DirectorySearch(wchar_t* inPath, u32& MAX, MemoryBlockArena*& currentArena, Array<wchar_t*>& foundFiles);
#else
	void DirectorySearch(char* directory, Array<char*>& foundFiles, MemoryBlockArena<char>* arena);
#endif // MR_PLATFORM_WINDOWS
}