/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(USEWIN)

#define	WIN32_LEAN_AND_MEAN
#define	NOMINMAX
#define NOSERVICE
#define NOMCX
#define NOHELP
#define NOKANJI

#define _WIN32_WINNT 0x0A00 // Windows 10
#include <Windows.h>

#endif // defined(_WIN32) || defined(_WIN64)