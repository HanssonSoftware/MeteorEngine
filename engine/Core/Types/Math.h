/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <cmath>
#include <HAL/DataTypes.h>

namespace Math
{
	template <typename T = float>
	static T EaseInOut(T num)
	{
		return (T)sin(1.5707963 * num);
	}

	template <typename T>
	static constexpr T Max(T value, T min, T max)
	{
		return value <= min ? min : value >= max ? max : value;
	}

	static constexpr const float PI = 3.14159265359f;

	static constexpr u32 Count(const char* data) noexcept
	{
		u32 count = 0;
		while (*data)
		{
			data++;
			count++;
		}

		return count;
	}
};
