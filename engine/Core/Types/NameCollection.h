/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <stdint.h>
#include "Name.h"

class String;

class NameCollection
{
	struct NameEntry
	{
		u64 hash;
		const char* name;
	};

public:
	NameEntry* array;

	static NameEntry LookUp(const String* value);

	const char* operator[](const String* index)
	{
		return "";
	}

	static constexpr Name GetEmptyName() { return ""; };
};

