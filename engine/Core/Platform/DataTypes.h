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

static constexpr const bool bIsUsingWide = true;


#elif MR_PLATFORM_LINUX
typedef signed char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

static constexpr const bool bIsUsingWide = false;

#else
#error No platform found! (DataTypes.h)
#endif // MR_PLATFORM_WINDOWS
