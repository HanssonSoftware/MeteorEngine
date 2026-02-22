/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#include <string>
#include <Platform/DataTypes.h>

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

	~String() noexcept;

	String(const char* Input);

	explicit String(int Input);

	explicit String(float Input);

	explicit String(u32 Input);

	String(const String& other);

	String(const char* string, u32 length);

	String(String&& other) noexcept;

	String& operator=(const String& other);

	String operator+(const String& Other);

	bool operator==(const String& Other) const
	{
		return strcmp(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other.bIsUsingHeap ? Other.heapBuffer.ptr : Other.stackBuffer.ptr) == 0;
	}

	bool operator==(const char* Other) const
	{
		return strcmp(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other) == 0;
	}
		
	bool operator!=(const String& Other) const
	{
		return strcmp(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other.bIsUsingHeap ? Other.heapBuffer.ptr : Other.stackBuffer.ptr) != 0;
	}

	bool operator!() const
	{
		return bIsUsingHeap ? !heapBuffer.ptr : !stackBuffer.ptr;
	}

	explicit operator bool() const
	{
		return Length() > 0;
	}

	String& operator+=(const String& other);

	String& operator+=(const char* other);
	
	const char* operator*() const
	{
		return bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr;
	}

	operator const char*() const
	{
		return bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr;
	}

	const char* Chr() const
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

	char* Data() { return bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr; };

private:

	void NullOut();

	char* DetermineLocation(u32 size);

	union
	{
		struct
		{
			char* ptr = nullptr;
			u32 length = 0;

			u32 capacity = 0;

		} heapBuffer;

		struct
		{
			char ptr[sizeof(heapBuffer) - sizeof(u16)] = {'\0'};

			u16 length = 0;

		} stackBuffer;
	};

	static constexpr u32 SSO_MAX_CHARS = sizeof(heapBuffer) - sizeof(u16) - 1;

	bool bIsUsingHeap = false;

#ifdef MR_DEBUG
	bool bIsInited = false;

	const char* functionWhereWasInited = nullptr;
#endif // MR_DEBUG
};

struct StringView
{
	StringView() = delete;

	StringView(const char* data, u32 length)
		: ptr(data)
		, size(length)
	{

	}

	virtual ~StringView() noexcept = default;

	const char* ptr = nullptr;

	const u32 size = 0;
};

//String operator+(const String& OtherA, const String& OtherB);
