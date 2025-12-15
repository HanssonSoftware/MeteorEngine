/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <stdint.h>


/** Human readable piece of text. */
class String
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

	String(const char* Input);

	String(const wchar_t* Input);

	String(int Input);

	String(float Input);

	String(uint32_t Input);

	String(const String& other);

	String(const char* string, uint32_t length);

	String(const wchar_t* string, uint32_t length);

	String(String&& other) noexcept;

	String& operator=(const String& other);

	String operator+(const String& Other);

	bool operator==(const String& Other) const
	{
		return wcscmp(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other.bIsUsingHeap ? Other.heapBuffer.ptr : Other.stackBuffer.ptr) == 0;
	}

	bool operator==(const wchar_t* Other) const
	{
		return wcscmp(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other) == 0;
	}
		
	bool operator!=(const String& Other) const
	{
		return wcscmp(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other.bIsUsingHeap ? Other.heapBuffer.ptr : Other.stackBuffer.ptr) != 0;
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

	String& operator+=(const char* other);
	
	const wchar_t* operator*() const
	{
		return bIsUsingHeap ? (heapBuffer.ptr || heapBuffer.length != 0 ? heapBuffer.ptr : L"") :
			stackBuffer.ptr || stackBuffer.length != 0 ? stackBuffer.ptr : L"";
	}

	operator const wchar_t*() const
	{
		return bIsUsingHeap ? (heapBuffer.ptr || heapBuffer.length != 0 ? heapBuffer.ptr : L"") :
			stackBuffer.ptr || stackBuffer.length != 0 ? stackBuffer.ptr : L"";
	}

	const wchar_t* Chr() const
	{
		return bIsUsingHeap ? (heapBuffer.ptr || heapBuffer.length != 0 ? heapBuffer.ptr : L"") :
			stackBuffer.ptr || stackBuffer.length != 0 ? stackBuffer.ptr : L"";
	}

	String Delim(const String character, bool first);

	bool IsEmpty() const noexcept
	{
		return bIsUsingHeap ? 
			heapBuffer.length == 0 || heapBuffer.ptr == nullptr : 
			stackBuffer.length == 0 || stackBuffer.ptr[0] == L'\0';
	}

	int ToInt() const noexcept
	{
		return std::wcstol(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, nullptr, 0);
	}

	float ToFloat() const noexcept
	{
		return std::wcstof(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, nullptr);
	}

	const uint32_t Length() const noexcept
	{
		return bIsUsingHeap ? (uint32_t)heapBuffer.length : (uint32_t)stackBuffer.length;
	}
	
	void Refresh() noexcept
	{
		if (heapBuffer.ptr != nullptr)
			heapBuffer.length = (uint32_t)wcslen(heapBuffer.ptr);

		stackBuffer.length = (uint32_t)wcslen(stackBuffer.ptr);
	}

	/** */
	static String Format(const String& format, ...);

	static bool Contains(const char* buffer, const char* target);

	wchar_t* Data() { return bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr; };

private:

	void NullOut()
	{
		// Advanced memory cleanup method required!

		bIsUsingHeap = false;

		heapBuffer.ptr = nullptr;
		heapBuffer.capacity = 0;
		heapBuffer.length = 0;

		wmemset(stackBuffer.ptr, 0, SSO_MAX_CHARS + 1);
		stackBuffer.length = 0;

#ifdef MR_DEBUG
		bIsInited = false;
#endif // MR_DEBUG
	}

	wchar_t* DetermineLocation(uint32_t size);

	union
	{
		struct
		{
			wchar_t* ptr = nullptr;

			uint32_t length = 0;

			uint32_t capacity = 0;

		} heapBuffer;

		struct
		{ 
			wchar_t ptr[sizeof(heapBuffer) - sizeof(uint32_t)] = {L'\0'};

			uint32_t length = 0;

		} stackBuffer;
	};

	static constexpr uint32_t SSO_MAX_CHARS = sizeof(heapBuffer) - sizeof(uint32_t) - 1;

	bool bIsUsingHeap = false;

#ifdef MR_DEBUG
	bool bIsInited = false;

	// enum instantiationStatus { None, Valid, NeedsClean, Error, Empty } status = None;

	const char* functionWhereWasInited = nullptr;
#endif // MR_DEBUG
};

String operator+(const String& OtherA, const String& OtherB);
