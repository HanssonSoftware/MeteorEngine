/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <stdint.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

/** Human readable piece of text. */
class CORE_API String
{
	friend class StringW;
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

	operator bool() const
	{
		return Length() > 0;
	}

	String& operator+=(const String& other);

	String& operator+=(const char* other);
	
	const char* operator*() const
	{
		return bIsUsingHeap ? (heapBuffer.ptr || heapBuffer.length != 0 ? heapBuffer.ptr : "") :
			stackBuffer.ptr || stackBuffer.length != 0 ? stackBuffer.ptr : "";
	}

	operator const char*() const
	{
		return bIsUsingHeap ? (heapBuffer.ptr || heapBuffer.length != 0 ? heapBuffer.ptr : "") :
			stackBuffer.ptr || stackBuffer.length != 0 ? stackBuffer.ptr : "";
	}

	const char* Chr() const
	{
		return bIsUsingHeap ? (heapBuffer.ptr || heapBuffer.length != 0 ? heapBuffer.ptr : "") :
			stackBuffer.ptr || stackBuffer.length != 0 ? stackBuffer.ptr : "";
	}

	bool IsEmpty() const noexcept
	{
		return bIsUsingHeap ? 
			heapBuffer.length == 0 || heapBuffer.ptr == nullptr : 
			stackBuffer.length == 0 || stackBuffer.ptr[0] == L'\0';
	}

	int ToInt() const noexcept
	{
		return std::stol(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, nullptr, 0);
	}

	float ToFloat() const noexcept
	{
		return std::stof(bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr, nullptr);
	}

	const uint32_t Length() const noexcept
	{
		return bIsUsingHeap ? (uint32_t)heapBuffer.length : (uint32_t)stackBuffer.length;
	}
	
	void Refresh() noexcept
	{
		if (heapBuffer.ptr != nullptr)
			heapBuffer.length = (uint32_t)strlen(heapBuffer.ptr);

		stackBuffer.length = (uint32_t)strlen(stackBuffer.ptr);
	}

	/** */
	static String Format(const String& format, ...);

	static bool Contains(const char* buffer, const char* target);

	char* Data() { return bIsUsingHeap ? heapBuffer.ptr : stackBuffer.ptr; };

private:

	void NullOut()
	{
		// Advanced memory cleanup method required!

		bIsUsingHeap = false;

		heapBuffer.ptr = nullptr;
		heapBuffer.capacity = 0;
		heapBuffer.length = 0;

		memset(stackBuffer.ptr, 0, SSO_MAX_CHARS + 1);
		stackBuffer.length = 0;

#ifdef MR_DEBUG
		bIsInited = false;
#endif // MR_DEBUG
	}

	char* DetermineLocation(uint32_t size);

	union
	{
		struct
		{
			char* ptr = nullptr;

			uint32_t length = 0;

			uint32_t capacity = 0;

		} heapBuffer;

		struct
		{ 
			char ptr[sizeof(heapBuffer) - sizeof(uint32_t)] = {L'\0'};

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
