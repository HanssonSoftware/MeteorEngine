/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "BaseProject.h"

void BaseProject::AddEntry(ProjectFileEntries* entry)
{
	includedEntries.Add(*entry);
}
