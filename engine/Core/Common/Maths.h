/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <cmath>

struct Math
{
	template <typename T = float>
	static T EaseInOut(T num);


	static constexpr const float GetPI() { return PI; };
private:
	static constexpr const float PI = 3.14159265359f;
};

template<typename T>
inline T Math::EaseInOut(T num)
{
	return (T)sin(1.5707963 * num);
}
