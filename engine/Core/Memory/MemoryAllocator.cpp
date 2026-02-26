

#include "MemoryAllocator.h"

static MemoryAllocator* instance = new MemoryAllocator;

MemoryAllocator* GetMemoryManager()
{
    return instance;
}
