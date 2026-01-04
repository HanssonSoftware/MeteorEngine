/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#include <basetsd.h>
#include <vector>
// #include <CoreProxy.h>

//
#include <Windows/Windows.h>

//	OC = Occupied // AV = Available
//	----------------------------------------------
//	|	OC	| AV |	OC	|						 |
//	----------------------------------------------

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

struct CORE_API MemoryManager
{
	static MemoryManager& Get();

	void Initialize(const double& RequiredMinimum);

	void Shutdown();

	template<typename T>
	T* Allocate(const uint64_t& size)
	{
		void* raw = begin + currentOffset;
		if (raw == nullptr)
			return nullptr;


		currentOffset += size * sizeof(T);
		return reinterpret_cast<T*>(raw);
	};

	template<typename T>
	void Deallocate(T* data)
	{
		if (!data)
			return;

		//data->~T();
		if (/*!VirtualFree(data, sizeof(T), MEM_DECOMMIT)*/false)
		{
			//MR_LOG(LogArena, Fatal, "%ls", *Platform::GetError());
		}

		//memset(data, 0xCD, sizeof(T));
		//currentOffset -= sizeof(T);
	};

protected:
	MemoryManager() = default;

	virtual ~MemoryManager() noexcept = default;

	struct MemoryData
	{
		/*
		--------------------------------------
		|	offset | INSTANCE OF THIS	size |
		--------------------------------------
		*/

		uint64_t offset = 0;

		uint64_t size = 0;

		bool used = false;
	};                            

	MemoryData FindAvailable(const uint64_t& size);

	uint32_t requiredMinimumInBytes = 1'000'000'000; // 1GB ~= 1000 MB

	uint64_t totalMemoryOnPC = 0;

	uint64_t currentOffset = 0;

	char* begin = nullptr;

	char* end = nullptr;

	//static inline std::vector<MemoryData> block;

	MemoryManager* object;
};
