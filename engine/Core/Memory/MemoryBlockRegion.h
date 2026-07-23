/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Logging/Log.h>
#include <HAL/DataTypes.h>
#include "MemoryBlockBase.h"

LOG_ADDCATEGORY(Memory);

#ifdef MR_CORE_EXPORT
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API MemoryBlockRegion : public MemoryBlockBase
{
	friend class MemoryHandler;

	struct Header
	{
		u64 size;
		u8* ptr;
	};

public:
	MemoryBlockRegion(u8* address, u64 size)
		: MemoryBlockBase(address, size)
	{
		if (size - sizeof(MemoryBlockRegion) < 8)
		{
			MR_LOG(LogMemory, Error, "Memory Region got very low amount of memory at: %x", ptr);
			return;
		}
	}

	void* Allocate(const u64 byte)
	{
		if (byte + offset + sizeof(Header) > size)
			return nullptr;

		void* chunk = (void*)(byte + offset + sizeof(Header));
		if (chunk != nullptr)
		{
			offset += byte + sizeof(Header);
			return chunk;
		}
		
		return nullptr;
	}

	void Dellocate(void* location)
	{

	}
};

