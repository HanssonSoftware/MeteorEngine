/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#ifdef MR_PLATFORM_WINDOWS
#define LIBRARY_OUT __declspec(dllexport)
#define LIBRARY_IN __declspec(dllimport)
#elif MR_PLATFORM_LINUX
#define LIBRARY_OUT
#define LIBRARY_IN
#error Implement for Linux!
#else
#error Unknown platform!
#endif // MR_PLATFORM_WINDOWS

