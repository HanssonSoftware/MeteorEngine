/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <cstdint>

struct IResourceAllocator
{
	virtual char* Allocate(uint64_t byte) = 0;
	virtual void Deallocate(uint64_t byte) = 0;

	virtual ~IResourceAllocator() noexcept = default;
};

