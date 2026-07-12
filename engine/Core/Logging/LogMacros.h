/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Math.h>

/** Creates a logging category, do NOT insert Logger before, just the word (automatically appends Log)! */
#define LOG_ADDCATEGORY(CategoryName) \
    /* Generated log name! */ \
    struct Log##CategoryName : public LogEntry \
	{ \
        static constexpr const char* GetName() { return #CategoryName; }; \
        static constexpr const wchar_t* GetNameW() { return L#CategoryName; }; \
        static constexpr const u32 Count = Math::Count(#CategoryName); \
    };

#define MR_LOG(CategoryName, severity, message, ...)
  

#ifdef MR_DEBUG
#define MR_ASSERT(expression, message, ...) 

#ifdef _WIN64
#endif // _WIN64
#else
#define MR_ASSERT(...) //* This function does nothing, if you want something, switch to Debug!
#endif // DEBUG
