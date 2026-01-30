/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#define _CRT_SECURE_NO_WARNINGS
#include "Commandlet.h"
#include <Types/String.h>
#include <Types/Array.h>
#include <Platform/PlatformLayout.h>

#define KERNEL
#define NLS
#include <Windows/Windows.h>
#include <Resource/MemoryManager.h>

#pragma warning(disable : 6031)

static inline Array<String> parsedWords;

void Commandlet::Initialize(int argumentCount, char argumentList[])
{
	if (argumentCount != -1) argumentsCount = argumentCount - 1;
	if (argumentList != nullptr) rawArgumentList = argumentList;

#ifdef MR_PLATFORM_WINDOWS
	const wchar_t* cli = GetCommandLineW();

	const int realSize = WideCharToMultiByte(CP_UTF8, 0, cli, -1, nullptr, 0, nullptr, nullptr);
	rawArgumentList = new char[realSize];
	memset(rawArgumentList, 0, realSize * sizeof(char));

	WideCharToMultiByte(CP_UTF8, 0, cli, -1, rawArgumentList, realSize * sizeof(char), nullptr, nullptr);
#endif // MR_PLATFORM_WINDOWS
	
	char* token = strtok(rawArgumentList, " ");
	while (token != nullptr)
	{
		// argumentsCount++;
		parsedWords.Add(token);
		token = strtok(nullptr, " ");
	}

	argumentsCount = parsedWords.GetSize();
	bIsInited = true;
}

void Commandlet::Shutdown()
{
#ifdef MR_PLATFORM_WINDOWS
	delete[] rawArgumentList;
#endif // MR_PLATFORM_WINDOWS

	bIsInited = false;
}

bool Commandlet::Parse(const String& inParam, String* returnVal)
{
	for (uint32_t i = 0; i < argumentsCount; i++)
	{
		String& item = parsedWords[i];

		if (item == inParam)
		{
			if (returnVal != nullptr && i + 1 <= argumentsCount + 1)
				*returnVal = parsedWords[i + 1];
			
			return true;
		}
	}

	return false;
}