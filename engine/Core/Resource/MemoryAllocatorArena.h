/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "Allocator.h"
#include <cstdint>

#include "MemoryManager.h"

class MemoryAllocatorArena
{
public:
	explicit MemoryAllocatorArena(uint64_t maximumAllocated)
	{
		
	}

	template<typename T>
	T* Allocate(uint64_t bytesToAllocate)
	{
		T* pointer = begin + offset;


		return nullptr;
	}

	bool Reset()
	{
		offset = 0;
		return true;
	}

	uint64_t offset = 0;

	char* begin = nullptr;
};


typedef MemoryAllocatorArena Arena;
