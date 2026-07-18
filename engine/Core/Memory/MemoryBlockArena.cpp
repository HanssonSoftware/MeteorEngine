/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "MemoryBlockArena.h"
#include <Memory/MemoryHandler.h>

MemoryBlockArena::~MemoryBlockArena() noexcept
{
	if (ptr)
	{
		//GetMemoryManager()->Deallocate(ptr, size);
	}
}

void* MemoryBlockArena::Allocate(const u64 bytes)
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

void* MemoryBlockArena::Exhaust()
{
	return Allocate(size - offset);
}

void MemoryBlockArena::Reset()
{
	offset = offsetMarker;
}

void MemoryBlockArena::SetMarker(const u64 marker)
{
	if (offset < size)
	{
		offsetMarker = marker;
	}
}
