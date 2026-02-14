/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "String.h"
#include <Logging/Log.h>

#include <Resource/MemoryManager.h>

#pragma warning(disable : 26495)
#pragma warning(disable : 5082) // second argument to 'va_start' is not the last named parameter

LOG_ADDCATEGORY(StringSet);


String::~String() noexcept
{
	if (bIsUsingHeap && heapBuffer.ptr)
		GetMemoryManager()->Deallocate(heapBuffer.ptr);

	NullOut();

#ifdef MR_DEBUG
	functionWhereWasInited = nullptr;
#endif // MR_DEBUG
}

String::String(const Char* Input)
{
	NullOut();

	if (!Input || *Input == '\0')
		return;

	const u32 size = (u32)LENGTH(Input);
	Char* target = DetermineLocation(size);

	STRCPY(target, Input, size);

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(int Input)
{
	//swprintf(stackBuffer.ptr, SSO_MAX_CHARS, L"%d", Input);

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(u32 Input)
{
	//sprintf(stackBuffer.ptr, SSO_MAX_CHARS, L"%ud", Input);

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(float Input)
{
	//sprintf(stackBuffer.ptr, SSO_MAX_CHARS, L"%f", Input);

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

	Char* determined = DetermineLocation(Length());
	MOVE(determined, other.Data(), Length());

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
		heapBuffer.ptr = GetMemoryManager()->Allocate<Char>(heapBuffer.capacity * CharSize);

		CLEAN(heapBuffer.ptr, heapBuffer.capacity);
		STRCPY(heapBuffer.ptr, other.heapBuffer.ptr, heapBuffer.length);
	}
	else
	{
		stackBuffer.length = other.stackBuffer.length;

		CLEAN(stackBuffer.ptr, stackBuffer.length);
		STRCPY(stackBuffer.ptr, other.stackBuffer.ptr, stackBuffer.length);
	}

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String::String(const Char* Input, u32 length)
{
	NullOut();

	if (!Input || *Input == '\0' || length <= 0)
		return;

	Char* direct = DetermineLocation(length);
	STRCPY(direct, Input, length);

#ifdef MR_DEBUG
	bIsInited = true;
	functionWhereWasInited = __FUNCSIG__;
#endif // MR_DEBUG
}

String String::operator+(const String& Other)
{
	//const Char* thisData = Data();
	//const Char* otherData = Other.Chr();

	//const uint32_t thisSize = LENGTH(thisData);
	//const uint32_t otherSize = LENGTH(otherData);

	//this->bIsUsingHeap = thisSize + otherSize > SSO_MAX_CHARS ? true : false;

	//if (this->bIsUsingHeap)
	//{
	//	ScopedPtr<Char> newBuffer = GetMemoryManager()->Allocate<Char>(thisSize + otherSize + 1u);
	//	memcpy(newBuffer.Get(), thisData, thisSize);
	//	memcpy(newBuffer.Get() + thisSize, otherData, otherSize);


	//	thisData = newBuffer.Get();
	//}
	//else
	//{

	//}

	return *this;
}

String& String::operator+=(const Char* other)
{
	if (!other)
		return *this;

	String tmp(other);
	return (*this += tmp);
}

String String::Format(const String& format, ...)
{
	va_list a;
	va_start(a, format.Chr());

	const Char* formattingBuffer = format.Chr();

	va_list a_cpy;
	va_copy(a_cpy, a);
	const int sizeForVA = VPRINTF(0, formattingBuffer, 0, a);
	va_end(a_cpy);

	Char* newFormattedBuffer = GetMemoryManager()->Allocate<Char>(sizeForVA + 1);

	const int result = VPRINTF(newFormattedBuffer, formattingBuffer, sizeForVA + 1, a);
	va_end(a);

	String stringized(newFormattedBuffer);

	GetMemoryManager()->Deallocate(newFormattedBuffer);
	return stringized;
}

void String::NullOut()
{
	// Advanced memory cleanup method required!

	bIsUsingHeap = false;

	heapBuffer.ptr = nullptr;
	heapBuffer.capacity = 0;
	heapBuffer.length = 0;

	CLEAN(stackBuffer.ptr, SSO_MAX_CHARS + 1);
	stackBuffer.length = 0;

#ifdef MR_DEBUG
	bIsInited = false;
#endif // MR_DEBUG
}

Char* String::DetermineLocation(u32 size)
{
	bIsUsingHeap = size > SSO_MAX_CHARS;
	if (bIsUsingHeap)
	{
		heapBuffer.capacity = size * 2;
		heapBuffer.ptr = GetMemoryManager()->Allocate<Char>(heapBuffer.capacity * CharSize);
		heapBuffer.length = size;

		CLEAN(heapBuffer.ptr, heapBuffer.capacity);
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
			heapBuffer.ptr = GetMemoryManager()->Allocate<Char>(heapBuffer.capacity * CharSize);

			CLEAN(heapBuffer.ptr, heapBuffer.capacity);
			STRCPY(heapBuffer.ptr, other.heapBuffer.ptr, heapBuffer.length);
		}
		else
		{
			stackBuffer.length = other.stackBuffer.length;
			
			CLEAN(stackBuffer.ptr, stackBuffer.length);
			STRCPY(stackBuffer.ptr, other.stackBuffer.ptr, stackBuffer.length);
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

	const u32 thisLen = Length();
	const u32 otherLen = other.Length();
	const u32 newLen = thisLen + otherLen;

	const Char* otherData = other.bIsUsingHeap ? other.heapBuffer.ptr : other.stackBuffer.ptr;

	if (bIsUsingHeap)
	{
		if (heapBuffer.capacity <= newLen)
		{
			const u32 newCap = newLen * 2;
			Char* newPtr = GetMemoryManager()->Allocate<Char>(newCap * CharSize);
			CLEAN(newPtr, newCap);

			if (heapBuffer.ptr && thisLen > 0)
				STRCPY(newPtr, heapBuffer.ptr, thisLen);

			STRCPY(newPtr + thisLen, otherData, otherLen);

			if (heapBuffer.ptr)
				GetMemoryManager()->Deallocate(heapBuffer.ptr);

			heapBuffer.ptr = newPtr;
			heapBuffer.capacity = newCap;
		}
		else
		{
			STRCPY(heapBuffer.ptr + thisLen, otherData, otherLen);
		}

		heapBuffer.length = newLen;
		heapBuffer.ptr[newLen] = '\0';
	}
	else
	{
		if (newLen <= SSO_MAX_CHARS)
		{
			STRCPY(stackBuffer.ptr + thisLen, otherData, otherLen);
			stackBuffer.length = newLen;
			stackBuffer.ptr[newLen] = '\0';
		}
		else
		{
			const uint32_t newCap = newLen * 2;
			Char* newPtr = GetMemoryManager()->Allocate<Char>(newCap * sizeof(Char));
			CLEAN(newPtr, newCap);

			if (thisLen > 0)
				STRCPY(newPtr, stackBuffer.ptr, thisLen);

			STRCPY(newPtr + thisLen, otherData, otherLen);

			heapBuffer.ptr = newPtr;
			heapBuffer.capacity = newCap;
			heapBuffer.length = newLen;

			bIsUsingHeap = true;
		}
	}

	return *this;
}