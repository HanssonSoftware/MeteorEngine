/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#ifdef MR_PLATFORM_WINDOWS
#include <wchar.h>
#define CLEAN(buffer, count) wmemset(buffer, L'\0', count)
#define STRCPY(destination, source, count) wcsncpy(destination, source, count)
#define VPRINTF(buffer, format, count, arglist) vswprintf(buffer, count, format, arglist)
#define LENGTH(source) wcslen(source)
#define COMPARE(what, with) wcscmp(what, with)
#define MOVE(where, to, count) wmemmove(to, where, count)
#else
#include <string>
#define CLEAN(buffer, count) memset(buffer, 0, count)
#define STRCPY(destination, source, count) strncpy(destination, source, count)
#define VPRINTF(buffer, format, count, arglist) vsnprintf(buffer, count, format, arglist)
#define LENGTH(source) strlen(source)
#define COMPARE(what, with) strcmp(what, with)
#define MOVE(where, to, count) memmove(to, where, count)
#endif // MR_PLATFORM_WINDOWS