/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/DataTypes.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

struct CORE_API IResourceAllocator
{
	virtual char* Allocate(u64 byte) = 0;
	virtual void Deallocate(u64 byte) = 0;

	virtual ~IResourceAllocator() noexcept = default;
};

