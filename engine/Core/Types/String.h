/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <string>
#include "StringView.h"
#include <Memory/MemoryHandler.h>
#include <HAL/DataTypes.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

// * Human readable piece of text
class CORE_API String
{
public:
	constexpr String();

	//String() noexcept
	//{
	//	NullOut();
	//}

	~String() noexcept;

	constexpr String(const char* Input);

	explicit String(int Input);

	explicit String(float Input);

	explicit String(u32 Input);

	String(const String& other);

	String(const char* string, u32 length);

	explicit String(const StringView& str);

	String(String&& other) noexcept;

	String& operator=(const String& other);

	constexpr String& operator=(const char* other);

	String operator+(const String& Other);

	String& operator=(const StringView& other);

	bool operator==(const String& Other) const
	{
		return strcmp(bIsUsingHeap ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr, Other.bIsUsingHeap ? Other.internalBuffers.heapBuffer.ptr : Other.internalBuffers.stackBuffer.ptr) == 0;
	}

	bool operator==(const char* Other) const
	{
		return strcmp(bIsUsingHeap ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr, Other) == 0;
	}
		
	bool operator!=(const String& other) const
	{
		return strcmp(bIsUsingHeap ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr, other.bIsUsingHeap ? other.internalBuffers.heapBuffer.ptr : other.internalBuffers.stackBuffer.ptr) != 0;
	}

	bool operator!() const
	{
		return bIsUsingHeap ? !internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr[0] != '\0';
	}

	explicit operator bool() const
	{
		return Length() > 0;
	}

	String& operator+=(const String& other);

	String& operator+=(const char* other);
	
	const char* operator*() const
	{
		return bIsUsingHeap ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr;
	}

	operator const char*() const
	{
		return bIsUsingHeap ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr;
	}

	const char* Chr() const
	{
		return bIsUsingHeap ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr;
	}

	bool IsEmpty() const noexcept
	{
		return bIsUsingHeap ? 
			internalBuffers.heapBuffer.length == 0 || internalBuffers.heapBuffer.ptr == nullptr : 
			internalBuffers.stackBuffer.length == 0 || internalBuffers.stackBuffer.ptr[0] == '\0';
	}

	//int ToInt() const noexcept
	//{
	//	return strtol(bIsUsingHeap ? internalBuffers.heapBuffer.ptr : internalBuffers.internalBuffers.stackBuffer.ptr, nullptr, 10);
	//}

	//float ToFloat() const noexcept
	//{
	//	return strtof(bIsUsingHeap ? internalBuffers.heapBuffer.ptr : internalBuffers.internalBuffers.stackBuffer.ptr, nullptr);
	//}

	const u32 Length() const noexcept
	{
		return bIsUsingHeap ? (u32)internalBuffers.heapBuffer.length : (u32)internalBuffers.stackBuffer.length;
	}
	
	// * 
	static String Format(const char* format, ...);

	char* Data() { return bIsUsingHeap ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr; };

	const char* begin() const { return Chr(); };
	const char* end() const { return Chr() + Length(); };

private:

	void NullOut();

	char* DetermineLocation(u32 size);

	static constexpr u32 SSO_MAX_CHARS = (sizeof(char*) + sizeof(u32) * 2) - sizeof(u8);
	
	union CORE_API memoryLayout
	{
		struct
		{
			char* ptr;
			u32 length;

			u32 capacity;

		} heapBuffer;

		struct
		{
			char ptr[SSO_MAX_CHARS] = {};

			u8 length;

		} stackBuffer;

		constexpr memoryLayout()
			: heapBuffer{}
		{

		}

	} internalBuffers;

	bool bIsUsingHeap = false;

	constexpr inline u32 Count(const char* toCount)
	{
		u32 count = 0;
		while (*toCount)
		{
			toCount++;
			count++;
		}

		return count;
	};

	constexpr inline char* GetRecommendedBufferBySize(const u32 size)
	{
		if (size > SSO_MAX_CHARS)
		{
			bIsUsingHeap = true;

			internalBuffers.heapBuffer.capacity = (u32)(size * 1.5f);
			internalBuffers.heapBuffer.length = size;

			internalBuffers.heapBuffer.ptr = (char*)GetMemoryManager()->Allocate(internalBuffers.heapBuffer.capacity);
			return internalBuffers.heapBuffer.ptr;
		}
		else
		{
			internalBuffers.stackBuffer.length = size;
			return internalBuffers.stackBuffer.ptr;
		}

		return nullptr;
	}
};

//String operator+(const String& OtherA, const String& OtherB);