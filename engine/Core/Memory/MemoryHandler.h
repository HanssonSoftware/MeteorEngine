/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/DataTypes.h>

class MemoryRegion;

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API MemoryHandler
{
public:
	MemoryHandler() = default;
	virtual ~MemoryHandler() noexcept;

	virtual bool Initialize();

	virtual void* Allocate(const u64 byte);
	virtual void Deallocate(void* location, const u64 byte);

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

extern CORE_API MemoryHandler* GetMemoryManager();
