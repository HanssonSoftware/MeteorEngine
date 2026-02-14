/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

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

	String* Parse(const String& inParam);

	bool Check(const String& inParam);

	const u32 GetArgumentsCount() const { return argumentsCount; }

	inline const Char* GetArgumentsList() const { return rawArgumentList; }

protected:
	bool bIsInited = false;

	u32 argumentsCount = 0;

	Char* rawArgumentList = nullptr;
};