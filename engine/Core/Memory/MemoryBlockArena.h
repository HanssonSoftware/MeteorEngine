/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "MemoryHandler.h"
#include <HAL/DataTypes.h>

template<typename T>
class MemoryBlockArena
{
public:
	MemoryBlockArena() = delete;
	MemoryBlockArena(const MemoryBlockArena&) = delete;
	MemoryBlockArena(MemoryBlockArena&&) = delete;

	MemoryBlockArena& operator=(const MemoryBlockArena&) = delete;
	MemoryBlockArena& operator=(MemoryBlockArena&&) = delete;

	MemoryBlockArena(const u64 size)
	{
		this->size = size;
		ptr = (T*)GetMemoryManager()->Allocate(size);
	}
		
	virtual ~MemoryBlockArena() noexcept
	{
		if (ptr)
		{
			GetMemoryManager()->Deallocate(ptr, size);
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
		offset = 0;
	}

protected:
	T* ptr = nullptr;
	u64 size = 0;
	u64 offset = 0;
};

