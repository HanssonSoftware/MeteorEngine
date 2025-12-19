/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <TEMPIMPORT.h>

template<typename T>
class CORE_API Iterator
{
public:

	Iterator() = default;

	Iterator(T* ptr)
		: ptr(ptr)
	{

	}

	T& operator*()
	{
		return *ptr;
	}

	T* operator->()
	{
		return ptr;
	}

	Iterator& operator++() 
	{
		ptr++;
		return *this;
	}

	friend bool operator== (const Iterator& a, const Iterator& b) { return a.ptr == b.ptr; };

	friend bool operator!= (const Iterator& a, const Iterator& b) { return a.ptr != b.ptr; };
protected:
	T* ptr;
};

