/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <DataTypes.h>

#ifdef MR_CORE_EXPORT
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS


struct CORE_API StringView
{
	constexpr StringView() noexcept = default;

	constexpr StringView(const char* data, u32 length)
		: ptr((u8*)data)
		, size(length)
	{

	}

	constexpr StringView(const char* data)
		: ptr((u8*)data)
		, size(Count((u8*)data))
	{

	}

	//constexpr StringView(const u8* data, u32 length)
	//	: ptr(data)
	//	, size(length)
	//{

	//}

	//constexpr StringView(const u8* data)
	//	: ptr(data)
	//	, size(Count(data))
	//{

	//}

	~StringView() = default;

	const u8* ptr = nullptr;

	u32 size = 0;

	constexpr const u8* begin() const { return ptr; };
	constexpr const u8* end() const { return ptr + size; };

private:
	static constexpr u32 Count(const u8* data) noexcept
	{
		u32 count = 0;
		while (*data)
		{
			data++;
			count++;
		}

		return count;
	}
};
