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
	uint64_t reservedMemory = 0;

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

	begin = (char*)VirtualAlloc(nullptr, reservedMemory, MEM_COMMIT, PAGE_READWRITE);
	if (!begin && reservedMemory != 0)
	{
		wchar_t chars[256] = { L'\0' };

		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), LANG_USER_DEFAULT, chars, 256, nullptr);
		
		MR_LOG(LogArena, Error, "%ls", chars);
	}

	end = (char*)((char*)begin + reservedMemory);

	uint64_t reservedQuarter = reservedMemory / 4ull;
	
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

bool MemoryManager::RequestResourceFromEngine(uint64_t size)
{
	return false;
}

bool MemoryManager::RequestResource(uint64_t size)
{
#ifdef MR_PLATFORM_WINDOWS
	//if (!VirtualAlloc((uint64_t*)begin + offset, size, MEM_COMMIT, PAGE_READWRITE))
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