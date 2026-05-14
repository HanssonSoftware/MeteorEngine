/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <DataTypes.h>

struct MemoryPackage
{
	void* address = nullptr;
	u64 size = 0;
	u16 version = 0;
	i16 referenceCount = 0;
	bool bUsed = false;
};

