/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <HAL/DataTypes.h>
#include <HAL/Memory.h>
#include <Types/StringView.h>
#include "MemoryPackage.h"
#include "MemoryBlockPool.h"

class MemoryBlockBase;

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class MemoryHandler;

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

extern CORE_API MemoryHandler* GetMemoryManager();

class CORE_API MemoryHandler
{
	friend class Logger;
public:
	typedef MemoryBlockPool MemoryRegion;

	MemoryHandler() = default;
	virtual ~MemoryHandler() noexcept;

	virtual bool Initialize();

	virtual void* Allocate(const u64 byte, MemoryRegion* whichRegion = GetMemoryManager()->GetProjectRegion());
	virtual void Deallocate(u32 id);

	template<typename T = MemoryBlockBase>
	T* RequestNewRegion(const StringView& regionName, const u64 newRegionSizeInBytes)
	{
		T* newRegion = nullptr;
		if (void* regionWithSelfContained = HAL::OSAlloc(nullptr, newRegionSizeInBytes + sizeof(T)))
		{
			newRegion = new(regionWithSelfContained) T;

			// 1. Get projectRegion's nextRegion
			// 2. Find the nextRegion is nullptr
			// 3. If yes then set the new value to the nextRegion (where null)

			MemoryBlockBase* last = projectRegion->next;
			while (last) last = last->next;

			last = newRegion;
		}

		return newRegion;
	}

	static inline constexpr u64 RoundToMemoryAlignment(u64 byte)
	{
		if (byte == 0)
			return 0;

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

	MemoryBlockPool* GetEngineRegion() const { return engineRegion; };
	MemoryBlockPool* GetProjectRegion() const { return projectRegion; };

protected:
	MemoryBlockPool* engineRegion = nullptr;
	MemoryBlockPool* projectRegion = nullptr;
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
