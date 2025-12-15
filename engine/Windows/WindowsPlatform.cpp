/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "WindowsPlatform.h"
#include <cstdint>

#include <Windows.h>
#include <Types/String.h>
#include <Application.h>

LOG_ADDCATEGORY(Platform);

ScopedPtr<wchar_t> WindowsPlatform::ConvertToWide(const char* skinny)
{
	const uint32_t skinnyLength = (uint32_t)MultiByteToWideChar(CP_UTF8, 0, skinny, -1, nullptr, 0);
	if (skinnyLength != 0)
	{
		ScopedPtr<wchar_t> buffer = MemoryManager::Get().Allocate<wchar_t>(skinnyLength + 1);

		if (!MultiByteToWideChar(CP_UTF8, 0, skinny, skinnyLength * sizeof(char), buffer.Get(), skinnyLength))
		{
			MR_LOG(LogPlatform, Error, "Failed to convert narrow characters: %ls", *GetError());
			return nullptr;
		}

		return buffer;
	}

	return nullptr;
}

ScopedPtr<char> WindowsPlatform::ConvertToNarrow(const wchar_t* fat)
{
	const uint32_t fatLength = (uint32_t)WideCharToMultiByte(CP_UTF8, 0, fat, -1, nullptr, 0, 0, 0);
	if (fatLength != 0)
	{
		ScopedPtr<char> buffer = MemoryManager::Get().Allocate<char>(fatLength + 1);

		if (!WideCharToMultiByte(CP_UTF8, 0, fat, -1, buffer.Get(), fatLength * sizeof(char), 0, 0))
		{
			MR_LOG(LogPlatform, Error, "Failed to convert wide characters: %ls", *GetError());
			return nullptr;
		}


		return buffer;
	}

	return nullptr;
}

String WindowsPlatform::GetError()
{
	const DWORD error = GetLastError();
	LPWSTR str = nullptr;

	const uint32_t returned = (uint32_t)FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS, 
		nullptr, 
		error, 
		LANG_USER_DEFAULT, 
		(LPWSTR)&str, 
		0, 
		nullptr
	);

	if (!returned)
		return "";

	str[returned - 2] = L'\0';
	//String end(str);

	LocalFree(str);
	return "end";
}

bool WindowsPlatform::IsRunningAnAnotherInstance()
{
	//HANDLE appMtx = CreateMutexW(nullptr, 1, GetApplication()->GetApplicationCodeName());

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//if (appMtx) CloseHandle(appMtx);	
		return true;
	}

	return false;
}