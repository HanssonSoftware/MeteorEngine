/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "MemoryHandler.h"
#include <Logging/Log.h>
#include <new>

#include "Win32/WinMin.h"

#include "MemoryBlockPool.h"
#include <memoryapi.h>

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
#ifdef MR_PLATFORM_WINDOWS
        MemoryBlockPool* lastRegion = projectRegion->nextRegion;
        while (lastRegion)
        {
            lastRegion = lastRegion->nextRegion;
            VirtualFree(lastRegion, 0, MEM_RELEASE);

            lastRegion = nullptr;
        }

        if (!VirtualFree(engineRegion, 0, MEM_RELEASE) && !VirtualFree(projectRegion, 0, MEM_RELEASE))
        {

        }
#endif // MR_PLATFORM_WINDOWS
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

    constexpr const u64 fixed512MB = 512_mB;
    constexpr const u64 regionHeaderSize = (2 * sizeof(MemoryBlockPool));

    void* startingAddress = VirtualAlloc(nullptr, fixed512MB + regionHeaderSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
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
#endif // MR_PLATFORM_WINDOWS

    return false;
}

void* MemoryHandler::Allocate(const u64 byte)
{
    MR_ASSERT(projectRegion, "Tried accessing to an invalid address, which has not been initialised yet!");

    const u64 rounded = RoundToMemoryAlignment(byte);

    if (projectRegion->offset + rounded > projectRegion->size)
        return nullptr;

    void* allocated = projectRegion->ptr + projectRegion->offset;
    if (!allocated)
    {
        return {};
    }

    memset(allocated, 0, rounded);
    projectRegion->offset += rounded;

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

bool MemoryHandler::RequestNewRegion(const u64 newRegionSizeInBytes)
{
    MemoryBlockPool* newRegion = nullptr;
    if (void* regionWithSelfContained = VirtualAlloc(nullptr, newRegionSizeInBytes + sizeof(MemoryBlockPool), MEM_COMMIT, PAGE_READWRITE))
    {
        newRegion = new(regionWithSelfContained) MemoryBlockPool((u8*)regionWithSelfContained + sizeof(MemoryBlockPool), newRegionSizeInBytes);

        MemoryBlockPool* last = projectRegion->nextRegion;
        while (last) last = last->nextRegion;

        last = newRegion;
    }

    return newRegion;
}

bool MemoryHandler::RequestNewEngineRegion(const u64 newRegionSizeInBytes)
{
    MemoryBlockPool* newRegion = nullptr;
    if (void* regionWithSelfContained = VirtualAlloc(nullptr, newRegionSizeInBytes + sizeof(MemoryBlockPool), MEM_COMMIT, PAGE_READWRITE))
    {
        newRegion = new(regionWithSelfContained) MemoryBlockPool((u8*)regionWithSelfContained + sizeof(MemoryBlockPool), newRegionSizeInBytes);
   
        MemoryBlockPool* last = engineRegion->nextRegion;
        while (last) last = last->nextRegion;
    }

    return newRegion;
}
