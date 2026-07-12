/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "DataTypes.h"

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

namespace HAL
{
	//* Calls your operating system specific memory allocation function.
	CORE_API void* OSAlloc(void* specificLocationToStart, const u64 size);

	//* Calls your operating system specific memory deallocation function. Be advised that if this called, your variable should NOT be used again.
	CORE_API void OSDealloc(void* location, const u64 size);
}

constexpr inline u64 RoundTo(const u64 value, u64 To)
{
	return (value + (To - 1ull)) & ~(To - 1ull);
}
