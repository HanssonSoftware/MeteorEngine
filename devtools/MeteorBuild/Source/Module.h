/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>

namespace Processing
{
	void ProcessEntireBuffer(char*& buffer);
};


struct Module
{
	String moduleName;

	String parent;

	static Module MakeModuleFromBuffer(const char* buffer);
};
