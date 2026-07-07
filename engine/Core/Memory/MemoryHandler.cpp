/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "MemoryHandler.h"
#include <Logging/Log.h>
#include <new>
#include <HAL/Memory.h>

#include "Win32/MinimalWin.h"

#include "MemoryBlockPool.h"

#ifdef MR_PLATFORM_WINDOWS
#include <memoryapi.h>
#endif // MR_PLATFORM_WINDOWS

LOG_ADDCATEGORY(Memory);

static MemoryHandler instance;

MemoryHandler* GetMemoryManager()
{
    return &instance;
}

MemoryHandler::~MemoryHandler() noexcept
{
    if (engineRegion && projectRegion)
    {
        MemoryBlockPool* lastRegion = projectRegion->nextRegion;
        while (lastRegion)
        {
            lastRegion = lastRegion->nextRegion;
            HAL::OSDealloc(lastRegion, lastRegion->size);

            lastRegion = nullptr;
        }

        HAL::OSDealloc(engineRegion, engineRegion->size);
        HAL::OSDealloc(projectRegion, projectRegion->size);
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
    constexpr const u64 regionHeaderSize = (2 * sizeof(MemoryBlockPool));

    void* startingAddress = HAL::OSAlloc(nullptr, fixed512MB + regionHeaderSize);
    if (!startingAddress)
    {
        MR_LOG(LogMemory, Fatal, "Failed to reserve %llu bytes!", fixed512MB);
        return false;
    }

    memset(startingAddress, 0, fixed512MB + regionHeaderSize);

    constexpr u64 engineSize = fixed512MB / 3ull;
    constexpr u64 projectSize = fixed512MB - engineSize;

    engineRegion = new(startingAddress) MemoryBlockPool((u8*)startingAddress + sizeof(MemoryBlockPool), engineSize);
    projectRegion = new((u8*)startingAddress + engineSize) MemoryBlockPool((u8*)startingAddress + sizeof(MemoryBlockPool) + engineSize, projectSize);

    return engineRegion && projectRegion;
}

void* MemoryHandler::Allocate(const u64 byte, MemoryRegion* whichRegion)
{
    MR_ASSERT(projectRegion, "Tried accessing to an invalid address, which has not been initialised yet!");

    const u64 rounded = RoundToMemoryAlignment(byte);

    if (whichRegion->offset + rounded > whichRegion->size)
        return nullptr;

    void* allocated = whichRegion->ptr + whichRegion->offset;
    if (!allocated)
        return nullptr;

    new(allocated) void*;

    memset(allocated, 0, rounded);
    whichRegion->offset += rounded;

    return allocated;
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

MemoryBlockPool* MemoryHandler::RequestNewRegion(const StringView& regionName, const u64 newRegionSizeInBytes)
{
    MemoryBlockPool* newRegion = nullptr;
    if (void* regionWithSelfContained = HAL::OSAlloc(nullptr, newRegionSizeInBytes + sizeof(MemoryBlockPool)))
    {
        newRegion = new(regionWithSelfContained) MemoryBlockPool((u8*)regionWithSelfContained + sizeof(MemoryBlockPool), newRegionSizeInBytes);

        MemoryBlockPool* last = projectRegion->nextRegion;
        while (last) last = last->nextRegion;

        last = newRegion;
    }

    return newRegion;
}