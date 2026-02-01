/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Name.h"
#include <Types/String.h>
#include "NameCollection.h"

constexpr Name::Name(const char* string)
{
}

Name::Name(const String* string)
{
	NameCollection::LookUp(string);
}

Name::Name(uint64_t hashVal)
{
	
}
