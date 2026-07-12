/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "MemoryBlockBase.h"
#include <HAL/DataTypes.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API MemoryBlockPool : public MemoryBlockBase
{
	friend class MemoryHandler;
public:
	MemoryBlockPool() = default;
	virtual ~MemoryBlockPool() noexcept = default;

	MemoryBlockPool(MemoryBlockPool&& old) = delete;
	MemoryBlockPool& operator=(MemoryBlockPool&& old) = delete;
	MemoryBlockPool& operator=(const MemoryBlockPool& old)
	{
		return *this;
	}

	MemoryBlockPool(u8* address, u64 regionSizeInBytes)
		: MemoryBlockBase(address, regionSizeInBytes)
	{
		u64 converted = Round(regionSizeInBytes);
		converted /= 8;

		int J = 43;
	}

protected:
	constexpr inline u64 Round(u64 value) { return (value + 7) & ~7; };
};

