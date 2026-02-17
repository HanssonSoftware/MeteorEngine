/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#include <Platform/DataTypes.h>
#include "MemoryRegion.h"
#include "MemoryPackage.h"

#include <Windows/Windows.h>

//	OC = Occupied // AV = Available
//	----------------------------------------------
//	|	OC	| AV |	OC	|						 |
//	----------------------------------------------

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API MemoryManager
{
	friend class MemoryAllocatorArena;
public:
	MemoryManager();
	virtual ~MemoryManager() noexcept = default;

	void Initialize();
	void Shutdown();

	bool RequestResourceFromEngine(u64 size);
	bool RequestResource(u64 size);

	template<typename T>
	MemoryPackage<T> Allocate(u64 size)
	{
		MemoryPackage<T> allocatedReady(this);
		if (projectResource.regionStart)
		{
			T* found = (T*)(projectResource.regionStart + projectResource.offset);
			if (!found)
			{
				return allocatedReady;
			}

			projectResource.offset += size;

			allocatedReady.internalData = (T*)found;
		}

		return allocatedReady;
	}

	template<typename T>
	void Deallocate(T* data)
	{
		
	}

protected:
	constexpr u64 CeilPow2(u64 x) noexcept;
	constexpr u64 FloorPow2(u64 x) noexcept;
	constexpr u64 AlignUp(u64 x, u64 a) noexcept;

	u64 minimumStartingMemory = 0;

	MemoryRegion engineResource;
	MemoryRegion projectResource;

	char* begin = nullptr;
	char* end = nullptr;
};

MemoryManager* GetMemoryManager();
