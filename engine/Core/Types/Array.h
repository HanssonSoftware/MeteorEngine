/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#include "Iterator.h"
#include <Memory/MemoryHandler.h>

#include <cstring>
#include <Logging/Log.h>

#include <vector>

template <typename T>
class Array final
{
public:
	Array()
	{
		for (u32 i = 0; i < size; i++)
		{
			new(container[i]) T();
		}
	};

	~Array()
	{
		for (u32 i = 0; i < size; i++)
		{
			container[i].~T();
		}
	};

	void Add(T&& elementToAdd)
	{
		const u32 newPos = size + 1;

		new (container[newPos]) T(elementToAdd);
	}

	void Add(const T& elementToAdd)
	{
		const u32 newPos = size + 1;

		new (container[newPos]) T(elementToAdd);
	}

	T* Data() noexcept { return container; }
	const T* Data() const noexcept { return container; }

	u32 GetSize() const { return size; }
	u32 GetCapacity() const { return capacity; }

	explicit operator bool() const { return size > 0; }

	T& operator[](u32 index)
	{
		MR_ASSERT(index < capacity, "Index out of range! Your pointed: %u  Border: %u", index, capacity);
		return container[index];
	}

	const T& operator[](u32 index) const
	{
		MR_ASSERT(index < capacity, "Index out of range! Your pointed: %u  Border: %u", index, capacity);
		return container[index];
	}

	T* operator&(u32 index)
	{
		MR_ASSERT(index < capacity, "");
		return &container[index];
	}

	T* begin() { return container; }
	T* end() { return container + size; }
	const T* begin() const { return container; }
	const T* end() const { return container + size; }

private:

	T* container = nullptr;

	u32 size = 0;

	u32 capacity = 2; // capacity = capacity + (capacity / 0.3)
}; 