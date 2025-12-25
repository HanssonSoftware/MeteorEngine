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

static inline Array<String> parsedWords;

void Commandlet::Initialize()
{
#ifdef MR_PLATFORM_WINDOWS
	const LPWSTR cli = GetCommandLineW();

	const int realSize = WideCharToMultiByte(CP_UTF8, 0, cli, -1, nullptr, 0, nullptr, nullptr);
	char* buffer = new char[realSize];
	memset(buffer, 0, realSize * sizeof(char));

	WideCharToMultiByte(CP_UTF8, 0, cli, -1, buffer, realSize * sizeof(char), nullptr, nullptr);
#endif // MR_PLATFORM_WINDOWS
	
	char* token = strtok(buffer, " ");
	while (token != nullptr)
	{
		parsedWords.Add(token);
		token = strtok(nullptr, " ");
	}

#ifdef MR_PLATFORM_WINDOWS
	delete[] buffer;
#endif // MR_PLATFORM_WINDOWS

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