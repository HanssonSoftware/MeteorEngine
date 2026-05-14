/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#include "Iterator.h"
#include <Memory/MemoryHandler.h>

#include <cstring>
#include <Logging/Log.h>

template <typename T>
class Array final
{
public:
	Array()
	{
		Reserve(2);
	}

	~Array()
	{
		Clear();
	}

	Array(u32 count)
	{
		Reserve(count);
		for (u32 i = 0; i < count; ++i) container[i] = T{};
		size = count;
	}

	Array(const Array& other)
	{
		Reserve(other.size);
		for (u32 i = 0; i < other.size; ++i) container[i] = other.container[i];
		size = other.size;
	}

	Array(Array&& other) noexcept
		: container(other.container), size(other.size), capacity(other.capacity)
	{
		other.container = nullptr;
		other.size = 0;
		other.capacity = 0;
	}

	Array& operator=(Array other) noexcept
	{
		swap(other);
		return *this;
	}

	void Add(const T& elem)
	{
		if (size >= capacity) 
			Reserve(RecalculateCapacity(capacity));

		container[size++] = elem;
	}

	void Add(T&& elem)
	{
		if (size >= capacity) 
			Reserve(RecalculateCapacity(capacity));
			
		memmove(&container[size++], &elem, sizeof(elem));
	}

	void Add(const T& elem, uint32_t at)
	{
		MR_ASSERT(at <= size, "Your index value is pointing out of the array!");
		if (size >= capacity) Reserve(RecalculateCapacity(capacity));
		for (u32 i = size; i > at; --i) container[i] = std::move(container[i - 1]);
		container[at] = elem;
	}

	void Swap(u32 from, u32 to)
	{
		if (from < capacity || to < capacity)
		{
			auto a = container[from];
			auto b = container[to];

			container[from] = b;
			container[to] = a;
		}
	}

	void Remove(uint32_t at)
	{
		MR_ASSERT(at < size, "");
		container[at] = T{};
	}

	void Pop(uint32_t at)
	{
		MR_ASSERT(at < size, "");
		for (uint32_t i = at; i + 1 < size; ++i) container[i] = std::move(container[i + 1]);
		container[size - 1].~T();
		--size;
	}

	void Resize(u32 newSize)
	{
		if (newSize > capacity) Reserve(RecalculateCapacity(newSize));
		if (newSize > size)
		{
			for (u32 i = size; i < newSize; ++i) container[i] = T{};
		}
		else
		{
			for (u32 i = newSize; i < size; ++i) container[i].~T();
		}
		size = newSize;
	}

	void Reserve(u32 newCap)
	{
		if (newCap < capacity) 
			return;

		T* dst = (T*)GetMemoryManager()->Allocate(newCap * sizeof(T));

		for (u32 i = 0; i < size; ++i) 
			memmove(&dst[i], &container[i], sizeof(T));

		GetMemoryManager()->Deallocate(container, capacity);
		container = dst;
		capacity = newCap;
	}

	void Reset()
	{
		for (u32 i = 0; i < size; ++i) 
			container[i] = T{};
	}

	void Clear()
	{
		for (u32 i = 0; i < size; ++i)
		{
			container[i].~T(); // don't mind int/float/bool??/etc. types, they do not have user defined destructor
		}
		size = 0;
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

	void swap(Array& other) noexcept
	{
		if (*this/* == other*/)
		{

		}


	}

private:
	inline u32 RecalculateCapacity(u32 newSize) noexcept
	{
		u32 finalValue, oldValue;
		finalValue = oldValue = newSize;

		finalValue = finalValue * 2;

		return finalValue == oldValue ? finalValue * 3 : finalValue;
	}

	T* container = nullptr;

	u32 size = 0;

	u32 capacity = 0; // capacity = capacity + (capacity / 0.3)
}; 