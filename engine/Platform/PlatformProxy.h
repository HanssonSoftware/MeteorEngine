/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once

#ifdef Platform_EXPORTS
#define PLATFORM_API __declspec(dllexport)
#else
#define PLATFORM_API __declspec(dllimport)
#endif // Platform_EXPORTS
