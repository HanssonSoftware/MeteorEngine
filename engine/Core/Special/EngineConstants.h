/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#define COMPANY_NAME_SPACE "Hansson Software"
#define COMPANY_NAME "HanssonSoftware"

#define WIDE_COMPANY_NAME_SPACE L"Hansson Software"
#define WIDE_COMPANY_NAME L"HanssonSoftware"

#define ENGINE_NAME_SPACE "Meteor Engine"
#define ENGINE_NAME "MeteorEngine"

#define WIDE_ENGINE_NAME_SPACE L"Meteor Engine"
#define WIDE_ENGINE_NAME L"MeteorEngine"

#ifdef MR_PLATFORM_WINDOWS
#define CORE_LIBRARY_NAME "core.dll"
#elif MR_PLATFORM_LINUX
#define CORE_LIBRARY_NAME "core.so"
#else
#define CORE_LIBRARY_NAME "core.so"
#endif // MR_PLATFORM_WINDOWS


