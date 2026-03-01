/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/DataTypes.h>

class MemoryRegion
{
	friend class MemoryHandler;
public:
	constexpr MemoryRegion() = default;
	/*virtual*/ ~MemoryRegion() noexcept = default;

	MemoryRegion(u8* address, u64 regionSizeInBytes)
		: ptr(address)
		, size(regionSizeInBytes)
	{

	}

protected:
	u8* ptr = nullptr;
	u64 offset = 0;

	MemoryRegion* nextRegion = nullptr;
	u64 size = 0;
};

