/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_WCSTOK
#include "Commandlet.h"
#include <Types/Array.h>
#include <Types/String.h>
#include <Platform/PlatformLayout.h>

#ifdef MR_PLATFORM_WINDOWS
#include <Windows/Windows.h>
#endif // MR_PLATFORM_WINDOWS
#include <MemoryManager.h>

#pragma warning(disable : 6031)

void Commandlet::Initialize()
{
	String commandLine;
#ifdef MR_PLATFORM_WINDOWS
	commandLine = GetCommandLineW();
#endif // MR_PLATFORM_WINDOWS
	
	wchar_t* token = wcstok(commandLine.Data(), L" ");
	while (token != nullptr)
	{
		parsedWords.Add(String(token));
		token = wcstok(nullptr, L" ");
	}

	bIsInited = true;
}

bool Commandlet::Parse(const String& inParam, String* returnVal)
{
	if (!bIsInited) Initialize();

	uint32_t count = parsedWords.GetSize();

	for (uint32_t i = 0; i < count; i++)
	{
		String& item = parsedWords[i];

		if (item == inParam)
		{
			if (returnVal != nullptr && i + 1 <= count)
				*returnVal = parsedWords[i + 1];
			
			return true;
		}
	}

	return false;
}