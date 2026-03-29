/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include "GeneratorUtils.h"

class BaseGenerator
{
public:

	virtual bool Compile() = 0;

	void AddEntry(ProjectFileEntries* entry);
protected:
	Array<ProjectFileEntries> includedEntries;
};

