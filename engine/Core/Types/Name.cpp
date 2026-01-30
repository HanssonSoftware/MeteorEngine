/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Name.h"
#include <Types/String.h>
#include "NameCollection.h"

Name::Name(const String* string)
{
	NameCollection::LookUp(string);
}

Name::Name(uint64_t hashVal)
{
	
}
