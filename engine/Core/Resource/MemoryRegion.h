/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <cstdint>
#include <Types/String.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API MemoryRegion
{
	friend class MemoryManager;

	MemoryRegion* prevRegion = nullptr;
	MemoryRegion* nextRegion = nullptr;

	uint8_t* regionStart = nullptr;
	uint8_t* regionEnd = nullptr;

public:
	MemoryRegion() = delete;
	~MemoryRegion() noexcept = default;
	MemoryRegion(MemoryRegion* prev, MemoryRegion* next, uint8_t* start, uint8_t* end)
		: prevRegion(prev)
		, nextRegion(next)
		, regionStart(start)
		, regionEnd(end)
	{
		regionSize = (uint32_t)(end - start);
	};

	bool CheckOccupation(uint32_t size) { return false; };

	MemoryRegion* GetPreviousRegion() const { return prevRegion; };
	MemoryRegion* GetNextRegion() const { return nextRegion; };

	uint8_t* GetRegionBegin() const { return regionStart; };
	uint8_t* GetRegionEnd() const { return regionEnd; };

	//String GetRegionName() const { return regionName; };
	
	uint32_t regionSize = 0;
	uint32_t offset = 0;
};

