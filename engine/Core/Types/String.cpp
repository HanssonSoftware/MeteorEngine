/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "String.h"
#include <Logging/Log.h>
//#include <stdio.h>

#include <Platform/Platform.h>
#include <Platform/PlatformLayout.h>
#include <Windows/Windows.h>

#include <MemoryManager.h>
//#include "Pointers.h"

#pragma warning(disable : 26495)
#pragma warning(disable : 5082) // second argument to 'va_start' is not the last named parameter

LOG_ADDCATEGORY(StringSet);


String::~String() noexcept
{
	if (bIsUsingHeap && heapBuffer.ptr)
		MemoryManager::Get().Deallocate<wchar_t>(heapBuffer.ptr);

	NullOut();

#ifdef MR_DEBUG
	functionWhereWasInited = nullptr;
#endif // MR_DEBUG
}

String::String(const char* Input)
{
	NullOut();

	if (!Input || *Input == '\0')
		return;



#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(const wchar_t* Input)
{
	NullOut();

	if (!Input || *Input == L'\0')
		return;

	const uint32_t size = (uint32_t)wcslen(Input);
	wchar_t* target = DetermineLocation(size);
	
	wcsncpy(target, Input, size);

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(int Input)
{
	swprintf(stackBuffer.ptr, SSO_MAX_CHARS, L"%d", Input);

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(uint32_t Input)
{
	swprintf(stackBuffer.ptr, SSO_MAX_CHARS, L"%ud", Input);

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(float Input)
{
	swprintf(stackBuffer.ptr, SSO_MAX_CHARS, L"%f", Input);

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(String&& other) noexcept
{
	NullOut();

	bIsUsingHeap = other.bIsUsingHeap;

	if (bIsUsingHeap)
	{
		heapBuffer.capacity = other.heapBuffer.capacity;
		heapBuffer.length = other.heapBuffer.length;
	}
	else
	{
		stackBuffer.length = other.stackBuffer.length;
	}

	wchar_t* determined = DetermineLocation(Length());
	wmemmove(determined, other, Length());

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(const String& other)
{
	NullOut();

	bIsUsingHeap = other.bIsUsingHeap;
	if (bIsUsingHeap)
	{
		heapBuffer.capacity = other.heapBuffer.capacity;
		heapBuffer.length = other.heapBuffer.length;
		heapBuffer.ptr = MemoryManager::Get().Allocate<wchar_t>(heapBuffer.capacity * sizeof(wchar_t));

		memset(heapBuffer.ptr, 0, heapBuffer.capacity);
		wcsncpy(heapBuffer.ptr, other.heapBuffer.ptr, heapBuffer.length);
	}
	else
	{
		stackBuffer.length = other.stackBuffer.length;

		memset(stackBuffer.ptr, 0, stackBuffer.length);
		wcsncpy(stackBuffer.ptr, other.stackBuffer.ptr, stackBuffer.length);
	}

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(const char* Input, uint32_t length)
{
	NullOut();

	if (!Input || *Input == '\0' || length <= 0)
		return;

//#ifdef MR_PLATFORM_WINDOWS
//	const uint32_t skinnyLength = (uint32_t)MultiByteToWideChar(CP_UTF8, 0, Input, length, nullptr, 0);
//	if (skinnyLength > 0)
//	{
//		wchar_t* redirectedData = DetermineLocation(skinnyLength);
//		if (!MultiByteToWideChar(CP_UTF8, 0, Input, skinnyLength * sizeof(char), redirectedData, skinnyLength))
//		{
//			MR_LOG(LogStringSet, Error, "MultiByteToWideChar returned: %ls", *Platform::GetError());
//			return;
//		}
//	}
//	else
//	{
//		MR_LOG(LogStringSet, Error, "MultiByteToWideChar returned: %ls", *Platform::GetError());
//		return;
//	}
//#endif // MR_PLATFORM_WINDOWS

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(const wchar_t* string, uint32_t length)
{
	NullOut();
	
	wchar_t* direct = DetermineLocation(length);
	wcsncpy(direct, string, length);

#ifdef MR_DEBUG
	bIsInited = true;	
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}


String String::operator+(const String& Other)
{
	//const char* thisData = Data();
	//const char* otherData = Other.Chr();

	//const uint32_t thisSize = strlen(thisData);
	//const uint32_t otherSize = strlen(otherData);

	//this->bIsUsingHeap = thisSize + otherSize > SSO_MAX_CHARS ? true : false;

	//if (this->bIsUsingHeap)
	//{
	//	ScopedPtr<char> newBuffer = MemoryManager::Get().Allocate<char>(thisSize + otherSize + 1u);
	//	memcpy(newBuffer.Get(), thisData, thisSize);
	//	memcpy(newBuffer.Get() + thisSize, otherData, otherSize);


	//	thisData = newBuffer.Get();
	//}
	//else
	//{

	//}

	return *this;
}

String& String::operator+=(const char* other)
{
	if (!other)
		return *this;

	String tmp(other);
	return (*this += tmp);
}

String String::Delim(const String character, bool first)
{
	if (character.IsEmpty())
		return "";

	//char* A = /*buffer*/nullptr;
	//char* B = strtok(A, character.Chr());
	return "";
	//return first ? B : A;
}

String String::Format(const String& format, ...)
{
	va_list a;
	va_start(a, format.Chr());

	const wchar_t* formattingBuffer = format.Chr();

	va_list a_cpy;
	va_copy(a_cpy, a);
	const int sizeForVA = vswprintf(nullptr, 0, formattingBuffer, a_cpy);
	va_end(a_cpy);

	wchar_t* newFormattedBuffer = MemoryManager::Get().Allocate<wchar_t>(sizeForVA + 1);

	const int result = vswprintf(newFormattedBuffer, sizeForVA + 1 ,formattingBuffer, a);
	va_end(a);

	String stringized(newFormattedBuffer);

	MemoryManager::Get().Deallocate(newFormattedBuffer);
	return stringized;
}

bool String::Contains(const char* buffer, const char* target)
{
	if (!buffer || !target)
		return false;

	//char* dupedBuffer = _strdup(buffer);

	//MR_ASSERT(strcmp(dupedBuffer, buffer) == 0, "Buffer duplication error!");

	//char* deconstedParam = const_cast<char*>(target);

	//char* found = strstr(dupedBuffer, deconstedParam);

	//const bool result = found ? true : false;

	//free(dupedBuffer);
	return false;
}

wchar_t* String::DetermineLocation(uint32_t size)
{
	bIsUsingHeap = size > SSO_MAX_CHARS;
	if (bIsUsingHeap)
	{
		heapBuffer.capacity = size * 2;
		heapBuffer.ptr = MemoryManager::Get().Allocate<wchar_t>(heapBuffer.capacity * sizeof(wchar_t));
		heapBuffer.length = size;

		wmemset(heapBuffer.ptr, 0, heapBuffer.capacity);
		return heapBuffer.ptr;
	}

	stackBuffer.length = size;
	return stackBuffer.ptr;
}

String& String::operator=(const String& other)
{
	if (this != &other)
	{
		bIsUsingHeap = other.bIsUsingHeap;
		if (bIsUsingHeap)
		{
			heapBuffer.capacity = other.heapBuffer.capacity;
			heapBuffer.length = other.heapBuffer.length;
			heapBuffer.ptr = MemoryManager::Get().Allocate<wchar_t>(heapBuffer.capacity * sizeof(wchar_t));

			memset(heapBuffer.ptr, 0, heapBuffer.capacity);
			wcsncpy(heapBuffer.ptr, other.heapBuffer.ptr, heapBuffer.length);
		}
		else
		{
			stackBuffer.length = other.stackBuffer.length;
			
			memset(stackBuffer.ptr, 0, stackBuffer.length);
			wcsncpy(stackBuffer.ptr, other.stackBuffer.ptr, stackBuffer.length);
		}

#ifdef MR_DEBUG
		bIsInited = true;
		functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
	}

	return *this;
}

String& String::operator+=(const String& other)
{
	if (other.IsEmpty())
		return *this;

	const uint32_t thisLen = Length();
	const uint32_t otherLen = other.Length();
	const uint32_t newLen = thisLen + otherLen;

	const wchar_t* otherData = other.bIsUsingHeap ? other.heapBuffer.ptr : other.stackBuffer.ptr;

	if (bIsUsingHeap)
	{
		if (heapBuffer.capacity <= newLen)
		{
			const uint32_t newCap = newLen * 2;
			wchar_t* newPtr = MemoryManager::Get().Allocate<wchar_t>(newCap * sizeof(wchar_t));
			wmemset(newPtr, 0, newCap);

			if (heapBuffer.ptr && thisLen > 0)
				wmemcpy(newPtr, heapBuffer.ptr, thisLen);

			wmemcpy(newPtr + thisLen, otherData, otherLen);

			if (heapBuffer.ptr)
				MemoryManager::Get().Deallocate<wchar_t>(heapBuffer.ptr);

			heapBuffer.ptr = newPtr;
			heapBuffer.capacity = newCap;
		}
		else
		{
			wmemcpy(heapBuffer.ptr + thisLen, otherData, otherLen);
		}

		heapBuffer.length = newLen;
		heapBuffer.ptr[newLen] = L'\0';
	}
	else
	{
		if (newLen <= SSO_MAX_CHARS)
		{
			wmemcpy(stackBuffer.ptr + thisLen, otherData, otherLen);
			stackBuffer.length = newLen;
			stackBuffer.ptr[newLen] = L'\0';
		}
		else
		{
			const uint32_t newCap = newLen * 2;
			wchar_t* newPtr = MemoryManager::Get().Allocate<wchar_t>(newCap * sizeof(wchar_t));
			wmemset(newPtr, 0, newCap);

			if (thisLen > 0)
				wmemcpy(newPtr, stackBuffer.ptr, thisLen);

			wmemcpy(newPtr + thisLen, otherData, otherLen);

			heapBuffer.ptr = newPtr;
			heapBuffer.capacity = newCap;
			heapBuffer.length = newLen;

			bIsUsingHeap = true;
		}
	}

	return *this;
}