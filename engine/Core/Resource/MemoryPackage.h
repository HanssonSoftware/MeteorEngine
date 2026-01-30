/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <stdint.h>

template<typename T>
struct MemoryPackage
{
	MemoryPackage(T* inputData)
		: internalData(inputData)
	{

	}

	virtual ~MemoryPackage() noexcept
	{

	};

	T* internalData = nullptr;

	operator T*()
	{
		return this->internalData;
	}

	operator T&()
	{
		return *this->internalData;
	}

	MemoryPackage* nextPackage = nullptr;

	uint64_t offset = 0;
};

