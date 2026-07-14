/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Math.h>
#include <HAL/HAL.h>

//* Creates a logging category
#define LOG_ADDCATEGORY(CategoryName) \
    const u16 Log##CategoryName = Logger::AddLogCategory(#CategoryName);

#define MR_LOG(CategoryName, severity, message, ...)                \
    do                                                              \
    {                                                               \
        if constexpr (bIsRunningDebugMode || LogSeverity::severity != LogSeverity::Fatal)\
        {                                                           \
           _MR_LOG_IMPL_STANDARD(CategoryName, severity, L##message, __VA_ARGS__)\
        }                                                           \
        else if (LogSeverity::severity == LogSeverity::Fatal)\
        {                                                           \
           _MR_LOG_IMPL_FATAL(CategoryName, severity, L##message, __VA_ARGS__)\
            HAL::FatalExit(-1);\
        }                                                           \
    } while (0);                                                    \

#ifdef MR_PLATFORM_WINDOWS
#define _MR_LOG_IMPL_STANDARD(CategoryName, severity, message, ...)  Logger::Get()->LogStandard(CategoryName, LogSeverity::severity, message, __VA_ARGS__);
#define _MR_LOG_IMPL_FATAL(CategoryName, severity, message, ...)  Logger::Get()->LogFatal(CategoryName, LogSeverity::severity, message, __FUNCTIONW__, __LINE__, __FILEW__, __VA_ARGS__);
#define _MR_LOG_IMPL_ASSERT(CategoryName, severity, message, ...)  Logger::Get()->LogAssert(/*CategoryName, LogSeverity::severity, message, __VA_ARGS__*/);
#else
#define _MR_LOG_IMPL_STANDARD(CategoryName, severity, message, ...)  Logger::Get()->LogStandard(CategoryName, LogSeverity::severity, message, __VA_ARGS__);
#define _MR_LOG_IMPL_FATAL(CategoryName, severity, message, ...)  Logger::Get()->LogFatal(CategoryName, LogSeverity::severity, message, __FUNCTION__, __LINE__, __FILE__, __VA_ARGS__);
#define _MR_LOG_IMPL_ASSERT(CategoryName, severity, message, ...)  Logger::Get()->LogAssert(/*CategoryName, LogSeverity::severity, message, __VA_ARGS__*/);
#endif // MR_PLATFORM_WINDOWS


#ifdef MR_DEBUG
#define MR_ASSERT(expression, message, ...) 

#ifdef _WIN64
#endif // _WIN64
#else
#define MR_ASSERT(...) //* This function does nothing, if you want something, switch to Debug!
#endif // DEBUG
