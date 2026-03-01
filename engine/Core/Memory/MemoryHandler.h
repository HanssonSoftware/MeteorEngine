/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/DataTypes.h>

class MemoryRegion;

class MemoryHandler
{
public:
	constexpr MemoryHandler() = default;
	virtual ~MemoryHandler() noexcept;

	virtual bool Initialise();

	virtual void* Allocate(const u64 byte);
	virtual void Deallocate(void* location, const u64 byte);


protected:
	MemoryRegion* engineRegion = nullptr;
	MemoryRegion* projectRegion = nullptr;
};

extern MemoryHandler* GetMemoryManager();
