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

static inline uint32_t SwitchOnMemory(uint64_t x)
{
	if (x > 32ull * 1024ull * 1024ull * 1024ull)
	{
		return 4;
	}
	else if (x >= 16ull * 1024ull * 1024ull * 1024ull)
	{
		return 3;
	}
	else if (x >= 8ull * 1024ull * 1024ull * 1024ull)
	{
		return 2;
	}
	else if (x >= 4ull * 1024ull * 1024ull * 1024ull)
	{
		return 1;
	}
	else if (x < 4ull * 1024ull * 1024ull * 1024ull)
	{
		return 0;
	}
	else
	{
		MR_ASSERT(false, "What to do at here?");
	}

	return 0;
}

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

	String ramSwitch;
	if (Commandlet::Get().Parse("-maxram", &ramSwitch))
	{
		bool bIsCommandWasValid = false;

		reservedMemory = (uint64_t)strtoul(ramSwitch, nullptr, 10) * 1024ull * 1024ull; // bytes
		if (reservedMemory == 0)
		{
			MR_LOG(LogArena, Warn, "Invalid parameter detail provided for: -maxram was: %s", *ramSwitch);
		}

		if (reservedMemory > (uint64_t)status.ullTotalPageFile || reservedMemory == 0)
		{
			switch (SwitchOnMemory((uint64_t)status.ullTotalPageFile))
			{
				case 0:
				{
					MessageBoxW(nullptr, L"Your computer does not have enough memory!", WIDE_ENGINE_NAME_SPACE, MB_ICONERROR | MB_OK);

					TerminateProcess(GetCurrentProcess(), UINT_MAX);
					return;
				}
				case 1:
				{
					reservedMemory = 256ull * 1024ull * 1024ull;
					break;
				}
				case 2:
				{
					reservedMemory = 512ull * 1024ull * 1024ull;
					break;
				}
				case 3:
				{
					reservedMemory = 1024ull * 1024ull * 1024ull;
					break;
				}
				case 4:
				{
					reservedMemory = 2ull * 1024ull * 1024ull * 1024ull;
					break;
				}
			}
		}
	}
	else
	{
		switch (SwitchOnMemory((uint64_t)status.ullTotalPageFile))
		{
			case 0:
			{
				MessageBoxW(nullptr, L"Your computer does not have enough memory!", WIDE_ENGINE_NAME_SPACE, MB_ICONERROR | MB_OK);

				TerminateProcess(GetCurrentProcess(), UINT_MAX);
				return;
			}
			case 1:
			{
				reservedMemory = 256ull * 1024ull * 1024ull;
				break;
			}
			case 2:
			{
				reservedMemory = 512ull * 1024ull * 1024ull;
				break;
			}
			case 3:
			{
				reservedMemory = 1024ull * 1024ull * 1024ull;
				break;
			}
			case 4:
			{
				reservedMemory = 2ull * 1024ull * 1024ull * 1024ull;
				break;
			}
		}
	}

	begin = (char*)VirtualAlloc(nullptr, reservedMemory, MEM_RESERVE, PAGE_READWRITE);
	MR_ASSERT(!begin || reservedMemory != 0, "Failed to reserve %d bytes!", reservedMemory);

	end = (char*)((char*)begin + reservedMemory);

	uint64_t reservedQuarter = (reservedMemory / 2) / 2;

	engineResource = MemoryRegion(nullptr, nullptr, begin, begin + reservedQuarter);
	projectResource = MemoryRegion(nullptr, nullptr, begin + reservedQuarter, end);


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

bool MemoryManager::AllocateToEngine(uint64_t size)
{
	return false;
}

bool MemoryManager::RequestResource(uint64_t size)
{
#ifdef MR_PLATFORM_WINDOWS
	if (!VirtualAlloc((uint64_t*)begin + offset, size, MEM_COMMIT, PAGE_READWRITE))
	{
		DWORD a = GetLastError();

		DebugBreak();
	}
	
	offset += size;

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


	return 0;
}

uint64_t MemoryManager::CalculateInitialFromParameters()
{
	MEMORYSTATUSEX longlong = { sizeof(MEMORYSTATUSEX) };
	GlobalMemoryStatusEx(&longlong);

	if (longlong.ullTotalPhys < 1 * 1024 * 1024 * 1024)
	{
		MessageBoxW(nullptr, L"Your computer does not have enough memory!", WIDE_ENGINE_NAME_SPACE, MB_ICONERROR | MB_OK);

		TerminateProcess(GetCurrentProcess(), UINT_MAX);
		return 0;
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

	return nullptr;
}
