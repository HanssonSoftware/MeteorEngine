/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
//#include <Platform/DataTypes.h>

class MemoryManager;

template<typename T>
struct MemoryPackage
{
	MemoryPackage(MemoryManager* owner)
		: owner(owner)
	{

	}

	virtual ~MemoryPackage() noexcept = default;

	T* internalData = nullptr;

	operator T*()
	{
		return this->internalData;
	}

	operator T&()
	{
		return *this->internalData;
	}

	MemoryManager* owner = nullptr;
};

