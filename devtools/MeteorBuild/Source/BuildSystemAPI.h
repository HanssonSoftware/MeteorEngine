/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Special/ImportHelpers.h>

#ifdef MR_BUILDSYSTEM_EXPORT
#define BUILDSYSTEM_API LIBRARY_OUT
#else
#define BUILDSYSTEM_API LIBRARY_IN
#endif // MR_CORE_EXPORTS