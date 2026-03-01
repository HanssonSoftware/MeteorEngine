/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "MemoryHandler.h"
#include <Logging/Log.h>
#include <new>

#include <Windows/Windows.h>

#include "MemoryRegion.h"

LOG_ADDCATEGORY(Memory);

static MemoryHandler* instance = new MemoryHandler();

MemoryHandler* GetMemoryManager()
{
    return instance;
}

MemoryHandler::~MemoryHandler() noexcept
{
    if (engineRegion && projectRegion)
    {
#ifdef MR_PLATFORM_WINDOWS
        if (!VirtualFree(engineRegion, 0, MEM_RELEASE) && !VirtualFree(projectRegion, 0, MEM_RELEASE))
        {

        }
#endif // MR_PLATFORM_WINDOWS
    }

    delete instance;
}

bool MemoryHandler::Initialise()
{
#ifdef MR_PLATFORM_WINDOWS
    constexpr const u64 fixed512MB = 512 * 1024 * 1024;
    constexpr const u64 regionHeaderSize = (2 * sizeof(MemoryRegion));

    void* startingAddress = VirtualAlloc(nullptr, fixed512MB + regionHeaderSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!startingAddress)
    {
        MR_LOG(LogMemory, Fatal, "Failed to reserve %llu bytes!", fixed512MB);
        return false;
    }

    constexpr u64 engineSize = fixed512MB / 3ull;
    engineRegion = new(startingAddress) MemoryRegion((u8*)startingAddress + sizeof(MemoryRegion), engineSize);
    projectRegion = new((u8*)startingAddress + engineSize) MemoryRegion((u8*)startingAddress + engineSize + sizeof(MemoryRegion), fixed512MB - engineSize);

    return engineRegion && projectRegion;
#endif // MR_PLATFORM_WINDOWS

    return false;
}

void* MemoryHandler::Allocate(const u64 byte)
{
    MR_ASSERT(projectRegion, "Tried accessing to an invalid address, which has not been initialised yet!");

    void* allocated = VirtualAlloc(projectRegion->ptr + projectRegion->offset, byte, MEM_COMMIT, PAGE_READWRITE);
    if (!allocated)
    {
        return nullptr;
    }

    projectRegion->offset += byte;
    return allocated;
}

void MemoryHandler::Deallocate(void* location, const u64 byte)
{
    VirtualFree(location, byte, MEM_DECOMMIT);
}
