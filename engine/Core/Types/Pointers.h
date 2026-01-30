/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "Resource/MemoryManager.h"
// #include <CoreProxy.h>
/**
 * ScopedPtr owns a pointer, so you don't have to think about deleting it.
 * When this thing goes out of scope, the pointer is gone too.
*/
template<typename T>
class /*CORE_API*/ ScopedPtr
{
public:
	ScopedPtr()
	{
		ptr = nullptr;
	};

	ScopedPtr(T* newPtr)
		: ptr(newPtr)
	{

	};
	
	~ScopedPtr() noexcept
	{
		if (ptr != nullptr)
		{
			//GetMemoryManager()->Deallocate<T>(ptr); //delete ptr;
			ptr = nullptr;
		}
	};

	T& operator*()
	{
		return *this->ptr;
	}

	T* Get() const
	{
		return this->ptr;
	}

	T* operator->() const
	{
		return this->ptr;
	}

	T** operator&()
	{
		return &this->ptr;
	}

	ScopedPtr(const ScopedPtr&) = delete;

	ScopedPtr& operator=(const ScopedPtr&) = delete;

	ScopedPtr(ScopedPtr&& other) noexcept
	{
		ptr = other.ptr;
		other.ptr = nullptr;
	}

	ScopedPtr& operator=(ScopedPtr&& other) noexcept
	{
		//if (this != &other)
		{
			if (ptr) {}

			ptr = other.ptr;
			other.ptr = nullptr;
		}

		return *this;
	}

	operator T*()
	{
		return ptr;
	}
private:
	T* ptr = nullptr;
};