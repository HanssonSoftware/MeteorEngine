/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "MemoryHandler.h"
#include <DataTypes.h>

template<typename T>
class MemoryBlockArena
{
public:
	constexpr MemoryBlockArena() = default;
	virtual ~MemoryBlockArena() noexcept
	{
		if (ptr)
		{
			GetMemoryManager()->Deallocate(ptr, size);
		}
	}

	MemoryBlockArena(const u64 size)
	{
		this->size = size;
		ptr = (T*)GetMemoryManager()->Allocate(size);
	}

	virtual void* Allocate(const u64 bytes)
	{
		if (ptr && ptr + bytes)
		{
			void* result = ptr + bytes;

			offset += bytes;
			return result;
		}

		return nullptr;
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

