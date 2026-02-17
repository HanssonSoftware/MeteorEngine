/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/DataTypes.h>

class String;

struct Name
{
	constexpr Name() = default;
	constexpr Name(const char* string);
	Name(const String* string);
	Name(u64 hashVal);

private:
	u64 hash = 0;
};