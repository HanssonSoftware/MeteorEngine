/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <type_traits>

template<typename F, typename T>
inline T* Cast(F* Obj)
{
	static_assert(std::is_base_of<F, T>::value, "Class must be inherit from it's base!");

	return (T*)Obj;
}
