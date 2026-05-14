/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/DataTypes.h>
#include "MemoryPackage.h"

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

	virtual MemoryPackage AllocateTracked(u64 bytes);

protected:
	MemoryRegion* engineRegion = nullptr;
	MemoryRegion* projectRegion = nullptr;

#ifdef MR_WITH_EDITOR
	static constexpr const u32 MAX_HANDLES = 8192;
#else
	static constexpr const u32 MAX_HANDLES = 4096;
#endif // MR_WITH_EDITOR

	struct MemoryTable
	{
		void* address = nullptr;
		u64 size = 0;
		u16 version = 0;
		i16 referenceCount = 0;
		bool bUsed = false;

		static constexpr MemoryTable Invalid() 
		{
			return { nullptr, U64_MAX, U16_MAX, -1, false };
		}
	};

	MemoryTable list[MAX_HANDLES];

	u32 availableIndex = 0;
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
