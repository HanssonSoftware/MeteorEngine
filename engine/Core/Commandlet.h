/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Logging/Log.h>
#include <Types/String.h>

class String;

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

	void Initialize(int argumentCount, char argumentList[]);

	void Shutdown();

	bool Parse(const String& inParam, String* returnVal);

	const uint32_t GetArgumentsCount() const { return argumentsCount; }

	inline const char* GetArgumentsList() const { return rawArgumentList; }

protected:
	bool bIsInited = false;

	uint32_t argumentsCount = 0;

	char* rawArgumentList = nullptr;
};