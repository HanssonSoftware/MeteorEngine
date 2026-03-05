/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#include "Iterator.h"
#include <stdint.h>

#include <Logging/Log.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

template <typename T>
class CORE_API Array
{
public:
	Array()
		: container(nullptr), size(0), capacity(0)
	{
		Reserve(2);
	}

	Array(u32 count)
		: container(nullptr), size(0), capacity(0)
	{
		Reserve(count);
		for (u32 i = 0; i < count; ++i) container[i] = T{};
		size = count;
	}

	Array(const Array& other)
		: container(nullptr), size(0), capacity(0)
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

	~Array()
	{
		Clear();
		//delete[] container;
	}

	void Add(const T& elem)
	{
		if (size >= capacity) Reserve(capacity ? capacity + RESIZE_PAD : 2);
		container[size++] = elem;
	}

	void Add(T&& elem)
	{
		if (size >= capacity) Reserve(capacity ? capacity + RESIZE_PAD : 2);
		container[size++] = std::move(elem);
	}

	void Add(const T& elem, u32 at)
	{
		MR_ASSERT(at <= size, "");
		if (size >= capacity) Reserve(capacity ? capacity + RESIZE_PAD : 2);
		for (u32 i = size; i > at; --i) container[i] = std::move(container[i - 1]);
		container[at] = elem;
		++size;
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

	void Remove(u32 at)
	{
		MR_ASSERT(at < size, "");
		container[at] = T{};
	}

	void Pop(u32 at)
	{
		MR_ASSERT(at < size, "");
		for (u32 i = at; i + 1 < size; ++i) container[i] = std::move(container[i + 1]);
		container[size - 1].~T();
		--size;
	}

	void Resize(u32 newSize)
	{
		if (newSize > capacity) Reserve(newSize);
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
		if (newCap <= capacity) return;
		T* dst = GetMemoryManager()->Allocate(newCap * sizeof(T));
		for (u32 i = 0; i < size; ++i) dst[i] = std::move(container[i]);
		GetMemoryManager()->Deallocate(container);
		container = dst;
		capacity = newCap;
	}

	void Reset()
	{
		for (u32 i = 0; i < size; ++i) container[i] = T{};
	}

	void Clear()
	{
		for (u32 i = 0; i < size; ++i) container[i].~T();
		size = 0;
	}

	T* Data() { return container; }
	const T* Data() const { return container; }

	u32 GetSize() const { return size; }
	u32 GetCapacity() const { return capacity; }

	T& operator[](u32 index)
	{
		MR_ASSERT(index < size, "");
		return container[index];
	}

	const T& operator[](u32 index) const
	{
		MR_ASSERT(index < size, "");
		return container[index];
	}

	explicit operator bool() const { return size > 0; }

	T* operator&(u32 index)
	{
		MR_ASSERT(index < size, "");
		return &container[index];
	}

	T* begin() { return container; }
	T* end() { return container + size; }
	const T* begin() const { return container; }
	const T* end() const { return container + size; }

	void swap(Array& other) noexcept
	{
		std::swap(container, other.container);
		std::swap(size, other.size);
		std::swap(capacity, other.capacity);
	}

private:
	u32 RESIZE_PAD = size * 0.5f;

	T* container;

	u32 size;

	u32 capacity;
}; 