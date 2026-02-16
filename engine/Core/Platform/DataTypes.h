/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

#ifdef MR_PLATFORM_WINDOWS
typedef signed char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef wchar_t Character;
typedef wchar_t Char;
typedef wchar_t C;

static constexpr const u64 CharSize = sizeof(C);
static constexpr const bool bIsUsingWide = true;

#define Len(x) wcslen(x);

#elif MR_PLATFORM_LINUX
typedef signed char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char Character;
typedef char Char;
typedef char C;

static constexpr const u64 CharSize = sizeof(C);

#define TEXT(x) #x
#define Len(x) strlen(x);

#else
#error No platform found! (DataTypes.h)
static constexpr const bool bIsUsingWide = false;
#endif // MR_PLATFORM_WINDOWS
