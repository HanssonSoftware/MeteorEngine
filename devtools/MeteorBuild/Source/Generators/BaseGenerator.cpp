/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "BaseGenerator.h"

void BaseGenerator::AddEntry(ProjectFileEntries* entry)
{
	includedEntries.Add(*entry);
}
