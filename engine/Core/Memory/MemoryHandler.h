/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <HAL/DataTypes.h>
#include "MemoryPackage.h"

class MemoryBlockPool;

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

struct MemoryMetadata
{
	void* ptr;

	static MemoryMetadata Invalid() { return {}; };
};

enum class MemoryAllocationTag : u16
{
	// In the future, this tag list would be expanded!!

	MEM_PHYSICS
};

class CORE_API MemoryHandler
{
public:
	typedef u32 MemoryIdentifier;
	typedef MemoryBlockPool MemoryRegion;

	MemoryHandler() = default;
	virtual ~MemoryHandler() noexcept;

	virtual bool Initialize();

	virtual void* Allocate(const u64 byte);
	virtual void Deallocate(u32 id);

	virtual bool RequestNewRegion(const u64 newRegionSizeInBytes);
	virtual bool RequestNewEngineRegion(const u64 newRegionSizeInBytes);

	static inline constexpr u64 RoundToMemoryAlignment(u64 byte)
	{
		if (byte == 0)
			return 1;

		byte--;
		byte |= byte >> 1;
		byte |= byte >> 2;
		byte |= byte >> 4;
		byte |= byte >> 8;
		byte |= byte >> 16;
		byte |= byte >> 32;
		byte++;

		return byte;
	}

protected:
	MemoryRegion* engineRegion = nullptr;
	MemoryRegion* projectRegion = nullptr;
};

constexpr u64 operator""_kB(u64 val)
{
	return val * 1024;
}

constexpr u64 operator""_mB(u64 val)
{
	return val * 1024 * 1024;
}

constexpr u64 operator""_gB(u64 val)
{
	return val * 1024 * 1024 * 1024;
}

extern CORE_API MemoryHandler* GetMemoryManager();
