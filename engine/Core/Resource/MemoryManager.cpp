/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "MemoryManager.h"
#include <Platform/PlatformLayout.h>
#include <Logging/Log.h>
#include <Commandlet.h>
#include <cstdlib>
//#include <Platform/Platform.h>

#include <Special/EngineConstants.h>

LOG_ADDCATEGORY(Arena);

static MemoryManager* object = new MemoryManager;
inline constexpr uint64_t minRam = /*8589934592u*/1 * 1024 * 1024 * 1024; /* Equivalent for 1GB */

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
	uint64_t initialMemory = ReadFromCommandline();
	if (initialMemory == 0)
	{
		initialMemory = CalculateInitialFromParameters();

	}

#ifdef MR_PLATFORM_WINDOWS
	begin = (uint8_t*)VirtualAlloc(nullptr, initialMemory, MEM_RESERVE, PAGE_READWRITE);
	if (!begin)
	{

	}

	end = (uint8_t*)begin + initialMemory;
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

uint64_t MemoryManager::ReadFromCommandline()
{
	String maxRam;
	uint64_t hasMaxRamSwitch = 0;
	if (Commandlet::Get().Parse("-maxram", &maxRam))
	{
		hasMaxRamSwitch = (uint64_t)strtoul(maxRam, nullptr, 10) * 1024 * 1024; // bytes
		if (hasMaxRamSwitch == 0)
		{
			MR_LOG(LogArena, Warn, "Invalid parameter detail provided for: -maxram was: %s", *maxRam);
		}

		return hasMaxRamSwitch;
	}

	return 0;
}

uint64_t MemoryManager::CalculateInitialFromParameters()
{
	MEMORYSTATUSEX longlong = { sizeof(MEMORYSTATUSEX) };
	GlobalMemoryStatusEx(&longlong);

	if (longlong.ullTotalPhys < minRam)
	{
		MessageBoxW(nullptr, L"Your computer does not have enough memory!", WIDE_ENGINE_NAME_SPACE, MB_ICONERROR | MB_OK);

		TerminateProcess(GetCurrentProcess(), UINT_MAX);
		return;
	}



	return 0;
}

constexpr uint64_t MemoryManager::CeilPow2(uint64_t x) noexcept
{
	if (x <= 1) return 1;
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;
	return x + 1;
}

constexpr uint64_t MemoryManager::FloorPow2(uint64_t x) noexcept
{
	if (x == 0) return 0;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x |= x >> 32;
	return x - (x >> 1);
}

constexpr uint64_t MemoryManager::AlignUp(uint64_t x, uint64_t a) noexcept
{
	return (x + (a - 1)) & ~(a - 1);
}

uint64_t MemoryManager::DetermineBestAmount()
{
#ifdef MR_PLATFORM_WINDOWS




	//return hasMaxRamSwitch > 0 ? longlong.ullTotalPageFile >= hasMaxRamSwitch ? hasMaxRamSwitch : 
	//	(uint64_t)longlong.ullTotalPageFile * 0.4 : 
	//	(uint64_t)longlong.ullTotalPageFile * 0.4;
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
