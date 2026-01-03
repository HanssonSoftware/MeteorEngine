/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "StringW.h"
#include <Logging/Log.h>

#include <MemoryManager.h>
#include <Types/String.h>

#pragma warning(disable : 26495)
#pragma warning(disable : 5082) // second argument to 'va_start' is not the last named parameter

LOG_ADDCATEGORY(StringSet);

StringW::StringW(const String* narrowString)
{
	NullOut();

	if (narrowString != nullptr)
	{
#ifdef MR_PLATFORM_WINDOWS
		const uint32_t narrowSize = narrowString->Length();
		wchar_t* actualPlace = DetermineLocation(narrowSize);

		if (!MultiByteToWideChar(CP_UTF8, 0, narrowString->Chr(), narrowSize, actualPlace, narrowSize))
			return;

#else
#error StringW is designed to work with Windows! Feel free to expand to other OS-es.
#endif // MR_PLATFORM_WINDOWS
	}

#ifdef MR_DEBUG
	Stats.bIsInited = true;
	Stats.functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

StringW::StringW(const char* Input)
{
	if (Input != nullptr)
	{
#ifdef MR_PLATFORM_WINDOWS
		const uint32_t narrowSize = (uint32_t)strlen(Input);
		wchar_t* actualPlace = DetermineLocation(narrowSize);

		if (!MultiByteToWideChar(CP_UTF8, 0, Input, narrowSize, actualPlace, narrowSize))
			return;

#else
#error StringW is designed to work with Windows! Feel free to expand to other OS-es.
#endif // MR_PLATFORM_WINDOWS
	}

#ifdef MR_DEBUG
	Stats.bIsInited = true;
	Stats.functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

StringW::~StringW() noexcept
{
	if (bIsUsingHeap && heapBuffer.ptr)
		MemoryManager::Get().Deallocate<wchar_t>(heapBuffer.ptr);

	NullOut();

#ifdef MR_DEBUG
	Stats.functionWhereWasInited = nullptr;
#endif // MR_DEBUG
}

StringW::StringW(const wchar_t* Input)
{
	NullOut();

	if (!Input || *Input == L'\0')
		return;

	const uint32_t size = (uint32_t)wcslen(Input);
	wchar_t* target = DetermineLocation(size);

	wcsncpy(target, Input, size);

#ifdef MR_DEBUG
	Stats.bIsInited = true;
	Stats.functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

StringW::StringW(StringW&& other) noexcept
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
	Stats.bIsInited = true;
	Stats.functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

StringW::StringW(const StringW& other)
{
	NullOut();

	bIsUsingHeap = other.bIsUsingHeap;
	if (bIsUsingHeap)
	{
		heapBuffer.capacity = other.heapBuffer.capacity;
		heapBuffer.length = other.heapBuffer.length;
		heapBuffer.ptr = MemoryManager::Get().Allocate<wchar_t>(heapBuffer.capacity * sizeof(wchar_t));

		wmemset(heapBuffer.ptr, 0, heapBuffer.capacity);
		wcsncpy(heapBuffer.ptr, other.heapBuffer.ptr, heapBuffer.length);
	}
	else
	{
		stackBuffer.length = other.stackBuffer.length;

		wmemset(stackBuffer.ptr, 0, stackBuffer.length);
		wcsncpy(stackBuffer.ptr, other.stackBuffer.ptr, stackBuffer.length);
	}

#ifdef MR_DEBUG
	Stats.bIsInited = true;
	Stats.functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

StringW::StringW(const wchar_t* Input, uint32_t length)
{
	NullOut();

	if (!Input || *Input == '\0' || length <= 0)
		return;

	wchar_t* direct = DetermineLocation(length);
	wcsncpy(direct, Input, length);

#ifdef MR_DEBUG
	Stats.bIsInited = true;
	Stats.functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

StringW StringW::operator+(const StringW& Other)
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

StringW StringW::Format(const StringW& format, ...)
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

	StringW stringized(newFormattedBuffer);

	MemoryManager::Get().Deallocate(newFormattedBuffer);
	return stringized;
}

wchar_t* StringW::DetermineLocation(uint32_t size)
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

StringW& StringW::operator=(const StringW& other)
{
	if (this != &other)
	{
		bIsUsingHeap = other.bIsUsingHeap;
		if (bIsUsingHeap)
		{
			heapBuffer.capacity = other.heapBuffer.capacity;
			heapBuffer.length = other.heapBuffer.length;
			heapBuffer.ptr = MemoryManager::Get().Allocate<wchar_t>(heapBuffer.capacity * sizeof(wchar_t));

			wmemset(heapBuffer.ptr, 0, heapBuffer.capacity);
			wcsncpy(heapBuffer.ptr, other.heapBuffer.ptr, heapBuffer.length);
		}
		else
		{
			stackBuffer.length = other.stackBuffer.length;
			
			wmemset(stackBuffer.ptr, 0, stackBuffer.length);
			wcsncpy(stackBuffer.ptr, other.stackBuffer.ptr, stackBuffer.length);
		}

#ifdef MR_DEBUG
		Stats.bIsInited = true;
		Stats.functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
	}

	return *this;
}

StringW& StringW::operator=(const String& other)
{
	NullOut();


#ifdef MR_PLATFORM_WINDOWS
	const uint32_t narrowSize = other.Length();
	wchar_t* actualPlace = this->DetermineLocation(narrowSize);

	if (!MultiByteToWideChar(CP_UTF8, 0, other.Chr(), narrowSize, actualPlace, narrowSize))
		return *this;

#else
#error StringW is designed to work with Windows! Feel free to expand to other OS-es.
#endif // MR_PLATFORM_WINDOWS

#ifdef MR_DEBUG
	Stats.bIsInited = true;
	Stats.functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
	return *this;
}

StringW& StringW::operator+=(const StringW& other)
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