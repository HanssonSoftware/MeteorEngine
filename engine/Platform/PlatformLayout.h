/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once

#ifdef _WIN64
#define MR_PLATFORM_WINDOWS 1
inline constexpr const bool bIsRunningOnWindows = true;
#else 
#define MR_PLATFORM_WINDOWS 0
inline constexpr const bool bIsRunningOnWindows = false;
#endif // _WIN64
