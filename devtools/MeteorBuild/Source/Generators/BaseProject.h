/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Types/String.h>
#include "GeneratorUtils.h"

class BaseProject
{
public:

	virtual bool Compile() = 0;

	void AddEntry(ProjectFileEntries* entry);

	String projectName;

	String projectParent;
protected:
	Array<ProjectFileEntries> includedEntries;
};

