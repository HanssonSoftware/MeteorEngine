/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#include <stdint.h>
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

	bool AllocateToEngine(uint64_t size);
	bool RequestResource(uint64_t size);

	template<typename T>
	MemoryPackage<T> Allocate(uint64_t size)
	{
		//if (projectResource->nextRegion) 
		//{
		//
		//};

		//void* allocated = VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);

		MemoryPackage<T> allocatedReady((T*)malloc(size));

		return allocatedReady;
	}

	template<typename T>
	void Deallocate(T* data)
	{
		free(data);
	}

	void FillBlocks(uint64_t blocksToFill, uint64_t size);
protected:
	bool CalculateStartingMembers();

	constexpr uint64_t CeilPow2(uint64_t x) noexcept;
	constexpr uint64_t FloorPow2(uint64_t x) noexcept;
	constexpr uint64_t AlignUp(uint64_t x, uint64_t a) noexcept;

	uint64_t DetermineBestAmount();

	MemoryRegion* FindNextAvailableRegion();

	uint64_t minimumStartingMemory = 0;

	char* begin = nullptr;
	char* end = nullptr;
};

MemoryManager* GetMemoryManager();
