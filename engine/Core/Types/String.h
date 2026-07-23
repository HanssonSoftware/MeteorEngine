/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <string>
#include "StringView.h"
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

	~String() noexcept;

	constexpr String(const char* Input);

	String(const String& other);

	String(const char* string, u32 length);

	String(const StringView& str);

	String(String&& other) noexcept;

	String& operator=(const String& other);

	constexpr String& operator=(const char* other);

	String operator+(const String& Other);

	String& operator=(const StringView& other);

	bool operator==(const String& Other) const
	{
		return strcmp(!SitsOnStack() ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr, !Other.SitsOnStack() ? Other.internalBuffers.heapBuffer.ptr : Other.internalBuffers.stackBuffer.ptr) == 0;
	}

	bool operator==(const char* Other) const
	{
		return strcmp(!SitsOnStack() ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr, Other) == 0;
	}
		
	bool operator!=(const String& other) const
	{
		return strcmp(!SitsOnStack() ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr, !other.SitsOnStack() ? other.internalBuffers.heapBuffer.ptr : other.internalBuffers.stackBuffer.ptr) != 0;
	}

	bool operator!() const
	{
		return !SitsOnStack() ? !internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr[0] != '\0';
	}

	explicit operator bool() const
	{
		return Length() > 0;
	}

	String& operator+=(const String& other);

	String& operator+=(const char* other);

	operator const char*() const
	{
		return !SitsOnStack() ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr;
	}

	const char* Chr() const
	{
		return !SitsOnStack() ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr;
	}

	bool IsEmpty() const noexcept
	{
		return !SitsOnStack() ? 
			internalBuffers.heapBuffer.length == 0 || internalBuffers.heapBuffer.ptr == nullptr : 
			internalBuffers.stackBuffer.length == 0 || internalBuffers.stackBuffer.ptr[0] == '\0';
	}

	const u32 Length() const noexcept
	{
		return !SitsOnStack() ? (u32)internalBuffers.heapBuffer.length : (u32)internalBuffers.stackBuffer.length;
	}
	
	// * 
	static String Format(const char* format, ...);

	char* Data() { return !SitsOnStack() ? internalBuffers.heapBuffer.ptr : internalBuffers.stackBuffer.ptr; };

	const char* begin() const { return Chr(); };
	const char* end() const { return Chr() + Length(); };

	bool SitsOnStack() const { return internalBuffers.stackBuffer.length < SSO_MAX_CHARS ? true : false; };

private:
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

			i8 length;

		} stackBuffer;

		constexpr memoryLayout()
			: heapBuffer{}
		{

		}

	} internalBuffers;

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

	constexpr char* GetRecommendedBufferBySize(const u32 size);
};
