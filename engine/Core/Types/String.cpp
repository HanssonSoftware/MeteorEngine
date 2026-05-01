/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "String.h"
#include <Logging/Log.h>

#include <Memory/MemoryHandler.h>

#pragma warning(disable : 26495)
#pragma warning(disable : 5082) // second argument to 'va_start' is not the last named parameter

LOG_ADDCATEGORY(StringSet);

// BE ADVISED!	Allocate/Deallocate asks for bytes, well char size is 1 byte everywhere I think?


String::~String() noexcept
{
	if (bIsUsingHeap && heapBuffer.ptr)
		GetMemoryManager()->Deallocate(heapBuffer.ptr, heapBuffer.capacity);

	NullOut();
}

String::String(const char* Input)
{
	NullOut();

	if (!Input || *Input == '\0')
		return;

	const u32 size = (u32)strlen(Input);
	char* target = DetermineLocation(size);

	strncpy(target, Input, size);
}

String::String(int Input)
{
	//swprintf(stackBuffer.ptr, SSO_MAX_CHARS, L"%d", Input);
}

String::String(u32 Input)
{
	//sprintf(stackBuffer.ptr, SSO_MAX_CHARS, L"%ud", Input);

}

String::String(float Input)
{
	//sprintf(stackBuffer.ptr, SSO_MAX_CHARS, L"%f", Input);
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

	char* determined = DetermineLocation(Length());
	memmove(determined, other.Data(), Length());

}

String::String(const String& other)
{
	NullOut();

	bIsUsingHeap = other.bIsUsingHeap;
	if (bIsUsingHeap)
	{
		heapBuffer.capacity = other.heapBuffer.capacity;
		heapBuffer.length = other.heapBuffer.length;
		heapBuffer.ptr = (char*)GetMemoryManager()->Allocate(heapBuffer.capacity);

		memset(heapBuffer.ptr, 0, heapBuffer.capacity);
		strncpy(heapBuffer.ptr, other.heapBuffer.ptr, heapBuffer.length);
	}
	else
	{
		stackBuffer.length = other.stackBuffer.length;

		memset(stackBuffer.ptr, 0, stackBuffer.length);
		strncpy(stackBuffer.ptr, other.stackBuffer.ptr, stackBuffer.length);
	}
}

String::String(const char* Input, u32 length)
{
	NullOut();

	if (!Input || *Input == '\0' || length <= 0)
		return;

	char* direct = DetermineLocation(length);
	strncpy(direct, Input, length);
}

String::String(const StringView& str)
{
	NullOut();

	if (!str.ptr || *str.ptr == '\0' || str.size <= 0)
		return;

	char* direct = DetermineLocation(str.size);
	strncpy(direct, str.ptr, str.size);
}

String String::operator+(const String& Other)
{
	//const char* thisData = Data();
	//const char* otherData = Other.Chr();

	//const uint32_t thisSize = LENGTH(thisData);
	//const uint32_t otherSize = LENGTH(otherData);

	//this->bIsUsingHeap = thisSize + otherSize > SSO_MAX_CHARS ? true : false;

	//if (this->bIsUsingHeap)
	//{
	//	ScopedPtr<char> newBuffer = (char*)GetMemoryManager()->Allocate(thisSize + otherSize + 1u);
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

String String::Format(const char* format, ...)
{
	va_list a;
	va_start(a, format);

	const int sizeForVA = vsnprintf(nullptr, 0, format, a);

	char fixedFormattingBuffer[256 + 1] = {'\0'};
	char* formattedBuffer = sizeForVA <= 256 ? fixedFormattingBuffer : (char*)GetMemoryManager()->Allocate(sizeForVA + 1);

	const int result = vsnprintf(formattedBuffer, sizeForVA + 1, format, a);
	va_end(a);

	String stringized(formattedBuffer);

	if (sizeForVA > 256 ) GetMemoryManager()->Deallocate(formattedBuffer, sizeForVA + 1);
	return stringized;
}

void String::NullOut()
{
	// Advanced memory cleanup method required!

	bIsUsingHeap = false;

	heapBuffer.ptr = nullptr;
	heapBuffer.capacity = 0;
	heapBuffer.length = 0;

	memset(stackBuffer.ptr, 0, SSO_MAX_CHARS + 1);
	stackBuffer.length = 0;
}

char* String::DetermineLocation(u32 size)
{
	bIsUsingHeap = size > SSO_MAX_CHARS;
	if (bIsUsingHeap)
	{
		heapBuffer.capacity = size * 2;
		heapBuffer.ptr = (char*)GetMemoryManager()->Allocate(heapBuffer.capacity);
		heapBuffer.length = size;

		memset(heapBuffer.ptr, 0, heapBuffer.capacity);
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
			heapBuffer.ptr = (char*)GetMemoryManager()->Allocate(heapBuffer.capacity);

			memset(heapBuffer.ptr, 0, heapBuffer.capacity);
			strncpy(heapBuffer.ptr, other.heapBuffer.ptr, heapBuffer.length);
		}
		else
		{
			stackBuffer.length = other.stackBuffer.length;
			
			memset(stackBuffer.ptr, 0, stackBuffer.length);
			strncpy(stackBuffer.ptr, other.stackBuffer.ptr, stackBuffer.length);
		}
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

	const char* otherData = other.bIsUsingHeap ? other.heapBuffer.ptr : other.stackBuffer.ptr;

	if (bIsUsingHeap)
	{
		if (heapBuffer.capacity <= newLen)
		{
			const u32 newCap = newLen * 2;
			char* newPtr = (char*)GetMemoryManager()->Allocate(newCap);
			memset(newPtr, 0, newCap);

			if (heapBuffer.ptr && thisLen > 0)
				strncpy(newPtr, heapBuffer.ptr, thisLen);

			strncpy(newPtr + thisLen, otherData, otherLen);

			if (heapBuffer.ptr)
				GetMemoryManager()->Deallocate(heapBuffer.ptr, heapBuffer.capacity);

			heapBuffer.ptr = newPtr;
			heapBuffer.capacity = newCap;
		}
		else
		{
			strncpy(heapBuffer.ptr + thisLen, otherData, otherLen);
		}

		heapBuffer.length = newLen;
		heapBuffer.ptr[newLen] = '\0';
	}
	else
	{
		if (newLen <= SSO_MAX_CHARS)
		{
			strncpy(stackBuffer.ptr + thisLen, otherData, otherLen);
			stackBuffer.length = newLen;
			stackBuffer.ptr[newLen] = '\0';
		}
		else
		{
			const uint32_t newCap = newLen * 2;
			char* newPtr = (char*)GetMemoryManager()->Allocate(newCap * sizeof(char));
			memset(newPtr, 0, newCap);

			if (thisLen > 0)
				strncpy(newPtr, stackBuffer.ptr, thisLen);

			strncpy(newPtr + thisLen, otherData, otherLen);

			heapBuffer.ptr = newPtr;
			heapBuffer.capacity = newCap;
			heapBuffer.length = newLen;

			bIsUsingHeap = true;
		}
	}

	return *this;
}