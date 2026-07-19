/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "MemoryBlockBase.h"
#include <HAL/DataTypes.h>

class MemoryHandler;

#ifdef MR_CORE_EXPORT
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API MemoryBlockArena : public MemoryBlockBase
{
public:
	MemoryBlockArena() = default;
	MemoryBlockArena(const MemoryBlockArena&) = delete;
	MemoryBlockArena(MemoryBlockArena&&) = delete;

	MemoryBlockArena& operator=(const MemoryBlockArena& other)
	{
		ptr = other.ptr;
		size = other.size;
		offset = other.offset;
		offsetMarker = other.offsetMarker;

		return *this;
	}

	MemoryBlockArena& operator=(MemoryBlockArena&& other) noexcept
	{
		int J =  3;

		return *this;
	}

	MemoryBlockArena(u8* address, u64 regionSizeInBytes)
		: MemoryBlockBase(address, regionSizeInBytes)
	{

	}

	virtual ~MemoryBlockArena() noexcept;

	virtual void* Allocate(const u64 bytes);
	
	virtual void* Exhaust();

	void Reset();

	void SetMarker(const u64 marker);

protected:
	u64 offsetMarker = 0;
};

