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

	char* regionStart = nullptr;
	char* regionEnd = nullptr;

public:
	constexpr MemoryRegion() = default;
	MemoryRegion(const MemoryRegion&) = delete;
	~MemoryRegion() noexcept = default;
	MemoryRegion(MemoryRegion* prev, MemoryRegion* next, char* start, char* end)
		: prevRegion(prev)
		, nextRegion(next)
		, regionStart(start)
		, regionEnd(end)
	{
		regionSize = (uint32_t)(end - start);
	};

	MemoryRegion* GetPreviousRegion() const { return prevRegion; };
	MemoryRegion* GetNextRegion() const { return nextRegion; };

	char* GetRegionBegin() const { return regionStart; };
	char* GetRegionEnd() const { return regionEnd; };

	//String GetRegionName() const { return regionName; };
	
	u64 regionSize = 0;
	u64 offset = 0;
};

