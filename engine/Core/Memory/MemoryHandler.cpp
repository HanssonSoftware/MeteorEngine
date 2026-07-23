/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "MemoryHandler.h"
#include <Memory/MemoryBlockBase.h>
#include "MemoryBlockPool.h"

#ifdef MR_PLATFORM_WINDOWS
#include "Win32/MinimalWin.h"
#include <memoryapi.h>
#endif // MR_PLATFORM_WINDOWS

static MemoryHandler instance;


MemoryHandler* GetMemoryManager()
{
    return &instance;
}

MemoryHandler::~MemoryHandler() noexcept
{
    if (engineRegion && projectRegion)
    {
        MemoryBlockBase* lastRegion = projectRegion->next;
        while (!lastRegion)
        {
            lastRegion = lastRegion->next;
        }

        HAL::OSDealloc(engineRegion, engineRegion->size);
        // HAL::OSDealloc(projectRegion, projectRegion->size);
    }
}

bool MemoryHandler::Initialize()
{
#ifdef MR_PLATFORM_WINDOWS
    MEMORYSTATUSEX ms = { sizeof(MEMORYSTATUSEX) };
    GlobalMemoryStatusEx(&ms);

    if (ms.ullAvailPhys < 512_mB)
    {
        MR_LOG(LogMemory, Fatal, "Your rig is running low on memory!");
        return false;
    }
#endif // MR_PLATFORM_WINDOWS

    constexpr const u64 fixed512MB = 512_mB;
    constexpr const u64 regionHeaderSize = (2 * sizeof(MemoryBlockRegion));

    void* startingAddress = HAL::OSAlloc(nullptr, fixed512MB + regionHeaderSize);
    if (!startingAddress)
    {
        MR_LOG(LogMemory, Fatal, "Failed to reserve %llu bytes!", fixed512MB);
        return false;
    }

    memset(startingAddress, 0, fixed512MB + regionHeaderSize);

    constexpr u64 engineSize = fixed512MB / 3ull;
    constexpr u64 projectSize = fixed512MB - engineSize;

    engineRegion = new(startingAddress) MemoryBlockRegion((u8*)startingAddress + sizeof(MemoryBlockRegion), engineSize);
    projectRegion = new((u8*)startingAddress + engineSize) MemoryBlockRegion((u8*)startingAddress + sizeof(MemoryBlockRegion) + engineSize, projectSize);

    return engineRegion && projectRegion;
}

void MemoryHandler::Deallocate(u32 id)
{
    //if (byte == 0)
    //{
    //    return;
    //}

    //memset(location, 0, byte);
    //VirtualFree(location, byte, MEM_DECOMMIT);
}

void MemoryHandler::Deallocate(void* id)
{
    if (projectRegion && id)
    {
        projectRegion->Deallocate(id);
    }
}
