/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "String.h"
#include <Logging/Log.h>
#include <cstdarg>

#include <Memory/MemoryHandler.h>

#pragma warning(disable : 26495)
#pragma warning(disable : 5082) // second argument to 'va_start' is not the last named parameter

LOG_ADDCATEGORY(StringSet);

// BE ADVISED!	Allocate/Deallocate asks for bytes, well char size is 1 byte everywhere I think?


constexpr String::String()
	: internalBuffers()
{

}

String::~String() noexcept
{
	if (!SitsOnStack())
	{
		GetMemoryManager()->Deallocate(internalBuffers.heapBuffer.ptr);
	}
}

constexpr String::String(const char* Input)
{
	char* target = GetRecommendedBufferBySize(Count(Input));
	StringCopy(Input, target);
}


String::String(String&& other) noexcept
{
	internalBuffers = other.internalBuffers;

	if (!other.SitsOnStack())
	{
		other.internalBuffers.heapBuffer.capacity = 0;
		other.internalBuffers.heapBuffer.length = 0;
	}
	else
	{
		other.internalBuffers.stackBuffer.length = 0;
	}
}

String::String(const String& other)
{
	if (!other.SitsOnStack())
	{
		internalBuffers.heapBuffer.capacity = other.internalBuffers.heapBuffer.capacity;
		internalBuffers.heapBuffer.length = other.internalBuffers.heapBuffer.length;
		internalBuffers.heapBuffer.ptr = GetMemoryManager()->Allocate<char>(internalBuffers.heapBuffer.capacity, GetMemoryManager()->GetProjectRegion());

		memset(internalBuffers.heapBuffer.ptr, 0, internalBuffers.heapBuffer.capacity);
		strncpy(internalBuffers.heapBuffer.ptr, other.internalBuffers.heapBuffer.ptr, internalBuffers.heapBuffer.length);
	}
	else
	{
		internalBuffers.stackBuffer.length = other.internalBuffers.stackBuffer.length;

		memset(internalBuffers.stackBuffer.ptr, 0, internalBuffers.stackBuffer.length);
		strncpy(internalBuffers.stackBuffer.ptr, other.internalBuffers.stackBuffer.ptr, internalBuffers.stackBuffer.length);
	}
}

String::String(const char* Input, u32 length)
{
	if (!Input || *Input == '\0' || length <= 0)
		return;

	char* direct = GetRecommendedBufferBySize(length);
	strncpy(direct, Input, length);
}

String::String(const StringView& str)
{
	if (!str.ptr || *str.ptr == '\0' || str.size <= 0)
		return;

	char* direct = GetRecommendedBufferBySize(str.size);
	strncpy(direct, (char*)str.ptr, str.size);
}

String String::operator+(const String& Other)
{
	//const char* thisData = Data();
	//const char* otherData = Other.Chr();

	//const uint32_t thisSize = LENGTH(thisData);
	//const uint32_t otherSize = LENGTH(otherData);

	//this->SitsOnStack() = thisSize + otherSize > SSO_MAX_CHARS ? true : false;

	//if (this->SitsOnStack())
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
	char* formattedBuffer = sizeForVA <= 256 ? fixedFormattingBuffer : GetMemoryManager()->Allocate<char>(sizeForVA + 1, GetMemoryManager()->GetProjectRegion());

	const int result = vsnprintf(formattedBuffer, sizeForVA + 1, format, a);
	va_end(a);

	String stringized(formattedBuffer);

	//if (sizeForVA > 256 ) GetMemoryManager()->Deallocate(formattedBuffer);
	return stringized;
}

constexpr char* String::GetRecommendedBufferBySize(u32 size)
{
	if (size > SSO_MAX_CHARS)
	{
		internalBuffers.heapBuffer.capacity = (u32)(size * 1.5f);
		internalBuffers.heapBuffer.length = size;

		internalBuffers.heapBuffer.ptr = GetMemoryManager()->Allocate<char>(internalBuffers.heapBuffer.capacity);
		return internalBuffers.heapBuffer.ptr;
	}
	else
	{
		internalBuffers.stackBuffer.length = size;
		return internalBuffers.stackBuffer.ptr;
	}

	return nullptr;
}

String& String::operator=(const String& other)
{
	if (this != &other)
	{
		if (!other.SitsOnStack())
		{
			internalBuffers.heapBuffer.capacity = other.internalBuffers.heapBuffer.capacity;
			internalBuffers.heapBuffer.length = other.internalBuffers.heapBuffer.length;
			internalBuffers.heapBuffer.ptr = GetMemoryManager()->Allocate<char>(internalBuffers.heapBuffer.capacity, GetMemoryManager()->GetProjectRegion());

			memset(internalBuffers.heapBuffer.ptr, 0, internalBuffers.heapBuffer.capacity);
			strncpy(internalBuffers.heapBuffer.ptr, other.internalBuffers.heapBuffer.ptr, internalBuffers.heapBuffer.length);
		}
		else
		{
			internalBuffers.stackBuffer.length = other.internalBuffers.stackBuffer.length;
			
			memset(internalBuffers.stackBuffer.ptr, 0, internalBuffers.stackBuffer.length);
			strncpy(internalBuffers.stackBuffer.ptr, other.internalBuffers.stackBuffer.ptr, internalBuffers.stackBuffer.length);
		}
	}

	return *this;
}

constexpr String& String::operator=(const char* other)
{
	const u32 otherSize = Count(other);

	char* target = GetRecommendedBufferBySize(otherSize);
	StringCopy(other, target);

	return *this;
}

String& String::operator=(const StringView& other)
{
	if (strcmp(Chr(), (char*)other.ptr) == 0)
		return *this;

	char* target = GetRecommendedBufferBySize(other.size);
	strncpy(target, (char*)other.ptr, other.size);

	return *this;
}

String& String::operator+=(const String& other)
{
	MR_ASSERT(false, "Broken function do not use!");

	if (other.IsEmpty())
		return *this;

	const u32 thisLen = Length();
	const u32 otherLen = other.Length();
	const u32 newLen = thisLen + otherLen;

	const char* otherData = other.SitsOnStack() ? other.internalBuffers.heapBuffer.ptr : other.internalBuffers.stackBuffer.ptr;

	if (!SitsOnStack())
	{
		if (internalBuffers.heapBuffer.capacity <= newLen)
		{
			const u32 newCap = newLen * 2;
			char* newPtr = GetMemoryManager()->Allocate<char>(newCap, GetMemoryManager()->GetProjectRegion());
			memset(newPtr, 0, newCap);

			if (internalBuffers.heapBuffer.ptr && thisLen > 0)
				strncpy(newPtr, internalBuffers.heapBuffer.ptr, thisLen);

			strncpy(newPtr + thisLen, otherData, otherLen);

			//if (internalBuffers.heapBuffer.ptr)
				//GetMemoryManager()->Deallocate(internalBuffers.heapBuffer.ptr, internalBuffers.heapBuffer.capacity);

			internalBuffers.heapBuffer.ptr = newPtr;
			internalBuffers.heapBuffer.capacity = newCap;
		}
		else
		{
			strncpy(internalBuffers.heapBuffer.ptr + thisLen, otherData, otherLen);
		}

		internalBuffers.heapBuffer.length = newLen;
		internalBuffers.heapBuffer.ptr[newLen] = '\0';
	}
	else
	{
		if (newLen <= SSO_MAX_CHARS)
		{
			strncpy(internalBuffers.stackBuffer.ptr + thisLen, otherData, otherLen);
			internalBuffers.stackBuffer.length = newLen;
			internalBuffers.stackBuffer.ptr[newLen] = '\0';
		}
		else
		{
			const uint32_t newCap = newLen * 2;
			char* newPtr = GetMemoryManager()->Allocate<char>(newCap * sizeof(char), GetMemoryManager()->GetProjectRegion());
			memset(newPtr, 0, newCap);

			if (thisLen > 0)
				strncpy(newPtr, internalBuffers.stackBuffer.ptr, thisLen);

			strncpy(newPtr + thisLen, otherData, otherLen);

			internalBuffers.heapBuffer.ptr = newPtr;
			internalBuffers.heapBuffer.capacity = newCap;
			internalBuffers.heapBuffer.length = newLen;

			//SitsOnStack() = true;
		}
	}

	return *this;
}

