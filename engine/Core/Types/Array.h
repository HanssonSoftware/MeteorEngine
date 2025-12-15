/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once

#include "Iterator.h"
#include <stdint.h>

#include <Logging/Log.h>
//#include <vector>

template <typename T>
class Array
{
public:
	Array()
		: container(nullptr), size(0), capacity(0)
	{
		Reserve(2);
	}

	Array(uint32_t count)
		: container(nullptr), size(0), capacity(0)
	{
		Reserve(count);
		for (uint32_t i = 0; i < count; ++i) container[i] = T{};
		size = count;
	}

	Array(const Array& other)
		: container(nullptr), size(0), capacity(0)
	{
		Reserve(other.size);
		for (uint32_t i = 0; i < other.size; ++i) container[i] = other.container[i];
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

	void Add(const T& elem, uint32_t at)
	{
		MR_ASSERT(at <= size, "");
		if (size >= capacity) Reserve(capacity ? capacity + RESIZE_PAD : 2);
		for (uint32_t i = size; i > at; --i) container[i] = std::move(container[i - 1]);
		container[at] = elem;
		++size;
	}

	void Swap(uint32_t from, uint32_t to)
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

	void Resize(uint32_t newSize)
	{
		if (newSize > capacity) Reserve(newSize);
		if (newSize > size)
		{
			for (uint32_t i = size; i < newSize; ++i) container[i] = T{};
		}
		else
		{
			for (uint32_t i = newSize; i < size; ++i) container[i].~T();
		}
		size = newSize;
	}

	void Reserve(uint32_t newCap)
	{
		if (newCap <= capacity) return;
		T* dst = new T[newCap];
		for (uint32_t i = 0; i < size; ++i) dst[i] = std::move(container[i]);
		delete[] container;
		container = dst;
		capacity = newCap;
	}

	void Reset()
	{
		for (uint32_t i = 0; i < size; ++i) container[i] = T{};
	}

	void Clear()
	{
		for (uint32_t i = 0; i < size; ++i) container[i].~T();
		size = 0;
	}

	T* Data() { return container; }
	const T* Data() const { return container; }

	uint32_t GetSize() const { return size; }
	uint32_t GetCapacity() const { return capacity; }

	T& operator[](uint32_t index)
	{
		MR_ASSERT(index < size, "");
		return container[index];
	}

	const T& operator[](uint32_t index) const
	{
		MR_ASSERT(index < size, "");
		return container[index];
	}

	operator bool() const { return size > 0; }

	T* operator&(uint32_t index)
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
	static constexpr uint32_t RESIZE_PAD = 4;

	T* container;

	uint32_t size;

	uint32_t capacity;
}; 