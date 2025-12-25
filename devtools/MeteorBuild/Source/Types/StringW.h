/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <stdint.h>

class StringW
{
	friend class String;
public:
	StringW() noexcept
	{
		NullOut();

#ifdef MR_DEBUG
		Stats.bIsInited = true;
#endif // MR_DEBUG
	}

	virtual ~StringW() noexcept;

	StringW(const String* narrowString);

	StringW(const wchar_t* Input);

	StringW(const StringW& other);

	StringW(const wchar_t* string, uint32_t length);

	StringW(StringW&& other) noexcept;

	StringW& operator=(const StringW& other);

	StringW& operator=(const String& other);

	StringW operator+(const StringW& Other);

	bool operator==(const StringW& Other) const
	{
		return wcscmp(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other.bIsUsingHeap ? Other.heapBuffer.ptr : Other.stackBuffer.ptr) == 0;
	}

	bool operator==(const wchar_t* Other) const
	{
		return wcscmp(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, Other) == 0;
	}
		
	bool operator!=(const StringW& Other) const
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

	StringW& operator+=(const StringW& other);

	//StringW& operator+=(const wchar_t* other);
	
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

	bool IsEmpty() const noexcept
	{
		return bIsUsingHeap ? 
			heapBuffer.length == 0 || heapBuffer.ptr == nullptr : 
			stackBuffer.length == 0 || stackBuffer.ptr[0] == L'\0';
	}

	int ToInt() const noexcept
	{
		return wcstol(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, nullptr, 0);
	}

	float ToFloat() const noexcept
	{
		return wcstof(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, nullptr);
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

	static StringW Format(const StringW& format, ...);

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
		Stats.bIsInited = false;
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
	struct
	{
		bool bIsInited = false;

		const char* functionWhereWasInited = nullptr;
	} Stats;
#endif // MR_DEBUG
};
