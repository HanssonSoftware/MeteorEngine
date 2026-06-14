/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/DataTypes.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API MemoryBlockPool
{
	friend class MemoryHandler;
public:
	MemoryBlockPool() = default;
	virtual ~MemoryBlockPool() noexcept = default;
	MemoryBlockPool(MemoryBlockPool&& old) = delete;
	MemoryBlockPool& operator=(MemoryBlockPool&& old) = delete;
	MemoryBlockPool& operator=(const MemoryBlockPool& old) = delete;


	MemoryBlockPool(u8* address, u64 regionSizeInBytes)
		: ptr(address)
		, size(regionSizeInBytes)
	{

	}

protected:
	u8* ptr = nullptr;
	u64 offset = 0;

	MemoryBlockPool* nextRegion = nullptr;
	u64 size = 0;
};

