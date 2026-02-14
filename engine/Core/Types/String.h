/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <Platform/DataTypes.h>

#include "StringMacros.inl"

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

/** Human readable piece of text. */
class CORE_API String
{
public:
	String() noexcept
	{
		NullOut();

#ifdef MR_DEBUG
		bIsInited = true;
#endif // MR_DEBUG
	}

	virtual ~String() noexcept;

	String(const Char* Input);

	String(int Input);

	String(float Input);

	String(u32 Input);

	String(const String& other);

	String(const Char* string, u32 length);

	String(String&& other) noexcept;

	String& operator=(const String& other);

	String operator+(const String& Other);

	bool operator==(const String& Other) const
	{
		return COMPARE(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other.bIsUsingHeap ? Other.heapBuffer.ptr : Other.stackBuffer.ptr) == 0;
	}

	bool operator==(const Char* Other) const
	{
		return COMPARE(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other) == 0;
	}
		
	bool operator!=(const String& Other) const
	{
		return COMPARE(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other.bIsUsingHeap ? Other.heapBuffer.ptr : Other.stackBuffer.ptr) != 0;
	}

	bool operator!() const
	{
		return bIsUsingHeap ? !heapBuffer.ptr : !stackBuffer.ptr;
	}

	operator bool() const
	{
		return Length() > 0;
	}

	String& operator+=(const String& other);

	String& operator+=(const Char* other);
	
	const Char* operator*() const
	{
		return bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr;
	}

	operator const Char*() const
	{
		return bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr;
	}

	const Char* Chr() const
	{
		return bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr;
	}

	bool IsEmpty() const noexcept
	{
		return bIsUsingHeap ? 
			heapBuffer.length == 0 || heapBuffer.ptr == nullptr : 
			stackBuffer.length == 0 || stackBuffer.ptr[0] == '\0';
	}

	//int ToInt() const noexcept
	//{
	//	return strtol(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, nullptr, 10);
	//}

	//float ToFloat() const noexcept
	//{
	//	return strtof(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, nullptr);
	//}

	const u32 Length() const noexcept
	{
		return bIsUsingHeap ? (u32)heapBuffer.length : (u32)stackBuffer.length;
	}
	
	/** */
	static String Format(const String& format, ...);

	Char* Data() { return bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr; };

private:

	void NullOut();

	Char* DetermineLocation(u32 size);

	union
	{
		struct
		{
			Char* ptr = nullptr;
			u32 length = 0;

			u32 capacity = 0;

		} heapBuffer;

		struct
		{
			Char ptr[sizeof(heapBuffer) - sizeof(u32)] = {'\0'};

			u32 length = 0;

		} stackBuffer;
	};

	static constexpr u32 SSO_MAX_CHARS = sizeof(heapBuffer) - sizeof(u32) - 1;

	bool bIsUsingHeap = false;

#ifdef MR_DEBUG
	bool bIsInited = false;

	const char* functionWhereWasInited = nullptr;
#endif // MR_DEBUG
};

struct StringView
{
	StringView() = delete;

	StringView(const Char* data, u32 length)
		: ptr(data)
		, size(length)
	{

	}

	virtual ~StringView() noexcept = default;

	const Char* ptr = nullptr;

	const u32 size = 0;
};

//String operator+(const String& OtherA, const String& OtherB);
