/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "MemoryBlockBase.h"
#include "MemoryHandler.h"
#include <HAL/DataTypes.h>

class MemoryBlockArena : public MemoryBlockBase
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
		return *this;
	}

	MemoryBlockArena(u8* address, u64 regionSizeInBytes)
		: MemoryBlockBase(address, regionSizeInBytes)
	{

	}

	virtual ~MemoryBlockArena() noexcept
	{
		if (ptr)
		{
			//GetMemoryManager()->Deallocate(ptr, size);
		}
	}

	virtual void* Allocate(const u64 bytes)
	{
		const u64 rounded = MemoryHandler::RoundToMemoryAlignment(bytes);

		if (ptr && (u64)(ptr + offset + rounded) <= (u64)(ptr + size))
		{
			void* result = ptr + offset;

			offset += rounded;
			return result;
		}

		return nullptr;
	}
	
	virtual void* Exhaust()
	{
		return Allocate(size - offset);
	}

	void Reset()
	{
		offset = offsetMarker;
	}

	void SetMarker(const u64 marker)
	{
		if (offset < size)
		{
			offsetMarker = marker;
		}
	}

protected:
	u64 offsetMarker = 0;
};

