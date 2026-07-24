/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "Iterator.h"
#include <Memory/MemoryHandler.h>
#include <Logging/Log.h>
#include <type_traits>

//* An array can store multiple elements of ONE exact type
template <typename T>
class Array
{
public:
	explicit Array(u32 count)
	{
		capacity = count;
		container = GetMemoryManager()->Allocate<T>(capacity * sizeof(T));
	}

	Array()
	{
		container = GetMemoryManager()->Allocate<T>(sizeof(T));
	};

	~Array()
	{
		for (u32 i = 0; i < size; i++)
		{
			container[i].~T();
		}

		GetMemoryManager()->Deallocate(container);

		container = nullptr;
		capacity = 1;
		size = 0;
	};

	Array(const Array<T>& copy)
	{		
		capacity = copy.capacity;
		size = copy.size;
		if (capacity > 0) 
		{
			container = GetMemoryManager()->Allocate<T>(capacity * sizeof(T), GetMemoryManager()->GetProjectRegion());

			for (u32 i = 0; i < size; i++) 
			{
				new (&container[i]) T(copy.container[i]);
			}
		}
	}

	Array(Array&& move) noexcept
		: container(move.container)
		, capacity(move.capacity)
		, size(move.size)
	{		
		move.container = nullptr;
		move.capacity = 1;
		move.size = 0;
	}

	Array& operator=(const Array<T>& old)
	{
		if (this != &old)
		{
			Clear();

			capacity = old.capacity;
			size = old.size;
			if (capacity > 0)
			{
				container = GetMemoryManager()->Allocate<T>(capacity * sizeof(T));

				for (u32 i = 0; i < size; i++)
				{
					new (&container[i]) T(old.container[i]);
				}
			}
		}

		return *this;
	}

	Array& operator=(Array<T>&& old)
	{
		if (*this != old)
		{
			//Clear();

			container = old.container;
			capacity = old.capacity;
			size = old.size;

			old.container = nullptr;
			old.capacity = 1;
			old.size = 0;
		}

		return *this;
	}

	void Add(T&& elementToAdd)
	{
		if (size + 1 > capacity)
			Resize(capacity * 2);

		container[size++] = std::move(elementToAdd);
	}

	void Add(const T& elementToAdd)
	{
		if (size + 1 > capacity)
			Resize(capacity * 2);

		new (&container[size++]) T(std::forward<T>(elementToAdd));
	}

	template<typename...Args>
	T& Add(Args&&... args)
	{
		if (size + 1 > capacity)
			Resize(capacity + (size / 2));

		container[size++] = T(std::forward<T>(args...));
		return container[size - 1];
	}

	void Resize(u32 newCount)
	{
		MR_ASSERT(capacity < newCount, "Old array cap is higher than new!");

		T* newBlock = GetMemoryManager()->Allocate<T>(newCount * sizeof(T));
		for (u32 i = 0; i < size; i++)
			newBlock[i] = container[i];

		GetMemoryManager()->Deallocate(container);

		container = newBlock;
		capacity = newCount;
	}

	//* Empties your array, after this you should not use it anymore
	void Clear()
	{
		if (container)
		{
			for (u32 i = 0; i < size; i++)
			{
				container[i].~T();
			}

			GetMemoryManager()->Deallocate(container);
			container = nullptr;
		}

		size = 0;
		capacity = 1;
	}

	T* Data() noexcept { return container; }
	const T* Data() const noexcept { return container; }

	u32 GetSize() const { return size; }
	u32 GetCapacity() const { return capacity; }

	explicit operator bool() const { return size > 0 && container != nullptr; }

	bool operator!=(const Array<T>& other)
	{
		return container != other.container ? true : false;
	}

	T& operator[](u32 index)
	{
		MR_ASSERT(index < size, "Index out of range! Your pointed: %u  Border: %u", index, capacity);
		return container[index];
	}

	const T& operator[](u32 index) const
	{
		MR_ASSERT(index < size, "Index out of range! Your pointed: %u  Border: %u", index, capacity);
		return container[index];
	}

	T* operator&(u32 index)
	{
		MR_ASSERT(index < size, "");
		return &container[index];
	}

	T* begin() { return container; }
	T* end() { return container + size; }
	const T* begin() const { return container; }
	const T* end() const { return container + size; }

private:
	T* container = nullptr;

	u32 size = 0;

	u32 capacity = 1;
}; 