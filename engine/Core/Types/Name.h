/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <stdint.h>

class String;

struct Name
{
	constexpr Name() = default;
	constexpr Name(const char* string);
	Name(const String* string);
	Name(uint64_t hashVal);

private:
	uint64_t hash = 0;
};