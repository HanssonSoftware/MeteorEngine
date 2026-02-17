/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "Allocator.h"
#include <cstdint>

#include "MemoryManager.h"

class MemoryAllocatorArena
{
public:
	explicit MemoryAllocatorArena(u64 maximumAllocated)
	{
		
	}

	template<typename T>
	T* Allocate(u64 bytesToAllocate)
	{
		T* pointer = begin + offset;


		return nullptr;
	}

	bool Reset()
	{
		offset = 0;
		return true;
	}

	u64 offset = 0;

	char* begin = nullptr;
};


typedef MemoryAllocatorArena Arena;
