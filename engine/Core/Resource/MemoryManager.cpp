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

MemoryManager::MemoryManager()
{

}

MemoryManager* GetMemoryManager()
{
	MR_ASSERT(object != nullptr, "Memory Manager is null!");
	return object;
}

void MemoryManager::Initialize()
{
#ifdef MR_PLATFORM_WINDOWS
	u64 reservedMemory = 0;

	MEMORYSTATUSEX status = { sizeof(MEMORYSTATUSEX) };
	GlobalMemoryStatusEx(&status);

	if (status.ullTotalPageFile > 8ull * 1024ull * 1024ull * 1024ull)
	{
		reservedMemory = 512ull * 1024ull * 1024ull;
	}
	else if (status.ullTotalPageFile > 4ull * 1024ull * 1024ull * 1024ull)
	{
		reservedMemory = 256ull * 1024ull * 1024ull;
	}

	begin = (char*)VirtualAlloc(nullptr, reservedMemory, MEM_RESERVE, PAGE_READWRITE);
	if (!begin && reservedMemory != 0)
	{
		wchar_t chars[256] = { L'\0' };

		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), LANG_USER_DEFAULT, chars, 256, nullptr);
		
		MR_LOG(LogArena, Error, "%ls", chars);
	}

	end = (char*)((char*)begin + reservedMemory);

	u64 reservedQuarter = reservedMemory / 4ull;
	
	engineResource = MemoryRegion(nullptr, nullptr, begin, (char*)(begin + reservedQuarter));
	projectResource = MemoryRegion(nullptr, nullptr, (char*)(begin + reservedQuarter), end);

#else
#error MemoryManager class (Initialize) is only implemented to windows.
#endif // MR_PLATFORM_WINDOWS
}

void MemoryManager::Shutdown()
{
	if (!VirtualFree(begin, 0, MEM_RELEASE))
	{
		MR_LOG(LogArena, Fatal, "VirtualFree failed!");
	}

	begin = end = nullptr;
	delete object;
}

bool MemoryManager::RequestResourceFromEngine(u64 size)
{
	return false;
}

bool MemoryManager::RequestResource(u64 size)
{
#ifdef MR_PLATFORM_WINDOWS
	if (!VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE))
	{
		DWORD a = GetLastError();

		DebugBreak();
	}
	
	//offset += size;

	return true;
#else
#error MemoryManager class (RequestResource) is only implemented to windows.
#endif // MR_PLATFORM_WINDOWS
	return false;
}

void* MemoryManager::Allocate(u64 size)
{
	MR_ASSERT(projectResource.regionStart, "Reserved block for project is nowhere to be found (it's null), perhaps not initalized?");

	void* currentlyAllocated = VirtualAlloc(projectResource.regionStart + projectResource.offset, size, MEM_COMMIT, PAGE_READWRITE);
	if (!currentlyAllocated)
	{
		MR_ASSERT(false, "Implement invalid allocated part!");
	}

	memset(projectResource.regionStart + projectResource.offset, 0x9, size);

	projectResource.offset += size;
	return currentlyAllocated;
}

void MemoryManager::Deallocate(void* data, const u64 size)
{
	VirtualFree(data, size, MEM_DECOMMIT);
	int j = 235;
}

constexpr u64 MemoryManager::CeilPow2(u64 x) noexcept
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

constexpr u64 MemoryManager::FloorPow2(u64 x) noexcept
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

constexpr u64 MemoryManager::AlignUp(u64 x, u64 a) noexcept
{
	return (x + (a - 1)) & ~(a - 1);
}