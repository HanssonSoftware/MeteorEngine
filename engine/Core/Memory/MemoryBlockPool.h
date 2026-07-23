/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "MemoryBlockBase.h"
#include <HAL/DataTypes.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API MemoryBlockPool : public MemoryBlockBase
{
	friend class MemoryHandler;

	struct FreeNode
	{
		FreeNode* next = nullptr;
	};
public:
	MemoryBlockPool() = default;
	virtual ~MemoryBlockPool() noexcept = default;

	MemoryBlockPool(MemoryBlockPool&& old) = delete;
	MemoryBlockPool& operator=(MemoryBlockPool&& old) = delete;
	MemoryBlockPool& operator=(const MemoryBlockPool& old)
	{
		return *this;
	}

	MemoryBlockPool(u8* address, u64 regionSizeInBytes)
		: MemoryBlockBase(address, regionSizeInBytes)
	{
		u32 countOfMaxNodes = (u32)(regionSizeInBytes / 8);

		u64 countOfMaxNodesBytes = countOfMaxNodes * sizeof(FreeNode);

		FreeNode* current = nextNode = (FreeNode*)address;
		u64 actual = 0;
		while (countOfMaxNodesBytes > (actual + sizeof(FreeNode)))
		{
			FreeNode* next = (FreeNode*)(address + actual + sizeof(FreeNode));
			current->next = next;

			current = next;
			actual += sizeof(FreeNode);
		}
	}
	
	//* Allocate from pool, byte param has no use!
	virtual void* Allocate(const u64 byte = 0) override
	{
		if (!nextNode)
			return nullptr;

		FreeNode* found = nextNode;
		nextNode = nextNode->next;

		return found;
	}

	virtual void Deallocate(void* location) override
	{
		if (!location)
			return;

		FreeNode* found = nextNode;
		nextNode = nextNode->next;
	}

protected:
	constexpr inline u64 Round(u64 value) { return (value + 7) & ~7; };

	FreeNode* nextNode;
};

