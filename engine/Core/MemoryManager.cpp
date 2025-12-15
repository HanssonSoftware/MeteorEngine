/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "MemoryManager.h"
#include <Platform/PlatformLayout.h>
#include <Logging/Log.h>
//#include <Platform/Platform.h>

#include <vector>

LOG_ADDCATEGORY(Arena);

static double engineRecommendedPercent = 0.15;

MemoryManager& MemoryManager::Get()
{
	static MemoryManager instance;
	return instance;
}

void MemoryManager::Initialize(const double& RequiredMinimum)
{
	engineRecommendedPercent = (RequiredMinimum > 0.0 && RequiredMinimum <= 1.0) ? RequiredMinimum : 0.1;

#ifdef MR_PLATFORM_WINDOWS
	MEMORYSTATUSEX longlong = {};
	longlong.dwLength = sizeof(MEMORYSTATUSEX);

	if (!GlobalMemoryStatusEx(&longlong))
	{
		MR_LOG(LogArena, Fatal, "GlobalMemoryStatusEx failed!");
	}

	totalMemoryOnPC = longlong.ullTotalPhys;

	uint64_t requiredByPercent = (uint64_t)(longlong.ullTotalPhys * engineRecommendedPercent);
	if (requiredByPercent < (uint64_t)requiredMinimumInBytes)
	{
		MR_LOG(LogArena, Fatal, "Your PC's memory is too low! Consider buying more RAM.");
	}

	begin = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, requiredByPercent);
	
	MR_ASSERT(begin != nullptr, "Failed to reserve, the engine recommended memory! Application exiting...");
#endif // MR_PLATFORM_WINDOWS

	end = (char*)begin + requiredByPercent;

}

void MemoryManager::Shutdown()
{
	if (!HeapFree(GetProcessHeap(), 0, begin))
	{
		MR_LOG(LogArena, Fatal, "VirtualFree failed!");
	}

	begin, end = nullptr;

	delete object;
}

MemoryManager::MemoryData MemoryManager::FindAvailable(const uint64_t& size)
{
	return MemoryData();
}
