/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "Allocator.h"

class MemoryAllocatorArena : public IResourceAllocator
{
public:
	virtual char* Allocate(uint64_t byte);
	virtual void Deallocate(uint64_t byte);



};

