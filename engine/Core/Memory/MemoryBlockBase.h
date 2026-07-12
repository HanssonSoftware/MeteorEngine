/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <HAL/DataTypes.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

// * Abstract Memory Block core features more than half lies here.
class CORE_API MemoryBlockBase
{
	friend class MemoryHandler;
public:
	MemoryBlockBase() = default;
	virtual ~MemoryBlockBase() noexcept = default;
	MemoryBlockBase(u8* address, u64 regionSizeInBytes)
		: ptr(address)
		, size(regionSizeInBytes)
	{

	}

protected:
	u8* ptr = nullptr;

	u64 size = 0;
	u64 offset = 0;

	MemoryBlockBase* next = nullptr;
};