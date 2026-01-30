/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "MemoryManager.h"
#include <Platform/PlatformLayout.h>
#include <Logging/Log.h>
#include <Commandlet.h>
#include <cstdlib>
//#include <Platform/Platform.h>


LOG_ADDCATEGORY(Arena);

static MemoryManager* object = new MemoryManager;

struct
{
	uint64_t offset = 0;
} Resource;

#ifdef MR_PLATFORM_WINDOWS

static HANDLE CreateMRNotify(MEMORY_RESOURCE_NOTIFICATION_TYPE NotificationType)
{
	switch (NotificationType)
	{
		case LowMemoryResourceNotification:
			break;
		case HighMemoryResourceNotification:
			break;	
	}

	return nullptr;
}

#endif // MR_PLATFORM_WINDOWS

MemoryManager* GetMemoryManager()
{
	MR_ASSERT(object != nullptr, "Memory Manager is null!");
	return object;
}

void MemoryManager::Initialize()
{
	totalMemory = DetermineBestAmount();

#ifdef MR_PLATFORM_WINDOWS
	begin = (uint8_t*)VirtualAlloc(nullptr, totalMemory, MEM_RESERVE, PAGE_READWRITE);

	MR_ASSERT(begin != nullptr, "Failed to reserve, the engine recommended memory! Application exiting...");

	end = (uint8_t*)begin + totalMemory;
#else
#error MemoryManager class (Initialize) is only implemented to windows.
#endif // MR_PLATFORM_WINDOWS

	FillBlocks(0, 0);
}

void MemoryManager::Shutdown()
{
	if (!VirtualFree(begin, 0, MEM_RELEASE))
	{
		MR_LOG(LogArena, Fatal, "VirtualFree failed!");
	}

	begin, end = nullptr;
	delete object;
}

bool MemoryManager::AllocateToEngine(uint64_t size)
{
	return false;
}

bool MemoryManager::RequestResource(uint64_t size)
{
#ifdef MR_PLATFORM_WINDOWS
	if (!VirtualAlloc((uint64_t*)begin + Resource.offset, size, MEM_COMMIT, PAGE_READWRITE))
	{

	}
	
	Resource.offset += size;

	return true;
#else
#error MemoryManager class (RequestResource) is only implemented to windows.
#endif // MR_PLATFORM_WINDOWS
	return false;
}

void MemoryManager::FillBlocks(uint64_t blocksToFill, uint64_t size)
{

}

uint64_t MemoryManager::AlignToNearest(uint64_t x, uint64_t y)
{
	return (x + y / 2) & ~(y - 1);
}

static constexpr uint64_t minRam = /*8589934592u*/1 * 1024 * 1024 * 1024; /* Equivalent for 8GB */

uint64_t MemoryManager::DetermineBestAmount()
{
#ifdef MR_PLATFORM_WINDOWS
	MEMORYSTATUSEX longlong = { sizeof(MEMORYSTATUSEX) };
	GlobalMemoryStatusEx(&longlong);

	if (longlong.ullTotalPhys < minRam)
	{
		MessageBoxW(nullptr, L"Your PC does not have enough memory! You need at least 1GB of RAM.", L"Engine error!", MB_OK);

		TerminateProcess(GetCurrentProcess(), UINT_MAX);
		return 0;
	}

	SYSTEM_INFO si = {};
	GetSystemInfo(&si);

	String maxRam;
	uint64_t hasMaxRamSwitch = 0;
	if (Commandlet::Get().Parse("-maxram", &maxRam))
	{
		hasMaxRamSwitch = (uint64_t)strtoul(maxRam, nullptr, 10) * 1024 * 1024; // bytes
		if (hasMaxRamSwitch == 0)
		{
			MR_LOG(LogArena, Warn, "Invalid parameter detail provided for: -maxram was: %s", *maxRam);
		}
	}

	return hasMaxRamSwitch > 0 ? 
		longlong.ullTotalPageFile >= hasMaxRamSwitch ? hasMaxRamSwitch : 
		longlong.ullTotalPageFile * 0.4 : 
		longlong.ullTotalPageFile * 0.4;
#else
#error MemoryManager class (DetermineBestAmount) is only implemented to windows.
#endif // MR_PLATFORM_WINDOWS
	return 0;
}

MemoryRegion* MemoryManager::FindNextAvailableRegion()
{
	MemoryRegion* currentRegion = projectResource;
	if (currentRegion != nullptr)
	{
		do
		{

		} while (currentRegion->GetNextRegion() != nullptr);
	}

	return nullptr;
}
