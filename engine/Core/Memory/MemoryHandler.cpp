/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "MemoryHandler.h"
#include <Logging/Log.h>
#include <new>

#include <Platform/Winapi.h>

#include "MemoryRegion.h"
#include <memoryapi.h>

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
        MemoryRegion* lastRegion = projectRegion->nextRegion;
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

    delete instance;
}

bool MemoryHandler::Initialize()
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
    constexpr u64 projectSize = fixed512MB - engineSize;

    engineRegion = new(startingAddress) MemoryRegion((u8*)startingAddress + sizeof(MemoryRegion), engineSize);
    projectRegion = new((u8*)startingAddress + engineSize) MemoryRegion((u8*)startingAddress + sizeof(MemoryRegion) + engineSize, projectSize);

    return engineRegion && projectRegion;
#endif // MR_PLATFORM_WINDOWS

    return false;
}

void* MemoryHandler::Allocate(const u64 byte)
{
    MR_ASSERT(projectRegion, "Tried accessing to an invalid address, which has not been initialised yet!");

    if (projectRegion->offset + byte > projectRegion->size)
        return nullptr;

    void* allocated = projectRegion->ptr + projectRegion->offset;
    if (!allocated)
    {
        return nullptr;
    }

    projectRegion->offset += byte;
    return allocated;
}

void MemoryHandler::Deallocate(void* location, const u64 byte)
{
    if (byte == 0)
    {
        return;
    }

    memset(location, 0, byte);
    //VirtualFree(location, byte, MEM_DECOMMIT);
}

bool MemoryHandler::RequestNewRegion(const u64 newRegionSizeInBytes)
{
    MemoryRegion* newRegion = nullptr;
    if (void* regionWithSelfContained = VirtualAlloc(nullptr, newRegionSizeInBytes + sizeof(MemoryRegion), MEM_COMMIT, PAGE_READWRITE))
    {
        newRegion = new(regionWithSelfContained) MemoryRegion((u8*)regionWithSelfContained + sizeof(MemoryRegion), newRegionSizeInBytes);

        MemoryRegion* last = projectRegion->nextRegion;
        while (last) last = last->nextRegion;

        last = newRegion;
    }

    return newRegion;
}

bool MemoryHandler::RequestNewEngineRegion(const u64 newRegionSizeInBytes)
{
    MemoryRegion* newRegion = nullptr;
    if (void* regionWithSelfContained = VirtualAlloc(nullptr, newRegionSizeInBytes + sizeof(MemoryRegion), MEM_COMMIT, PAGE_READWRITE))
    {
        newRegion = new(regionWithSelfContained) MemoryRegion((u8*)regionWithSelfContained + sizeof(MemoryRegion), newRegionSizeInBytes);
   
        MemoryRegion* last = engineRegion->nextRegion;
        while (last) last = last->nextRegion;
    }

    return newRegion;
}
