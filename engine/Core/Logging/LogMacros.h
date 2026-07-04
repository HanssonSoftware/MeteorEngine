/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Memory/MemoryHandler.h>
#include <type_traits>
#include <stdarg.h>
#include <stdio.h>
#include <HAL/DataTypes.h>
#include <cstdlib>
#include <Types/Math.h>

#define MERGE(x, y) x##y
#define MERGE2(x) #x



struct LogAssertion
{
    constexpr LogAssertion(const char* inputFile, u32 inputLine, const char* statement)
        : assertLocationInFile(inputFile)
        , assertLineInFile(inputLine)
        , assertStatement(statement)
    {

    }

    void SetMessage(const char* message, ...)
    {
        va_list d = nullptr;
        va_start(d, message);
        vsnprintf(this->message, bIsRunningDebugMode ? 256 : 128, message, d);
        va_end(d);
    }

    u32 assertLineInFile = 0;

    const char* assertLocationInFile = nullptr;

    const char* assertStatement = nullptr;

    char message[bIsRunningDebugMode ? 256 : 128] = { '\0' };
};

enum LogSeverity
{
    Log      = 1,
    Info      = Log,
    Verbose,
    Warn,
    Error,
    Fatal
};

enum LogFormatting
{
    Category,
    Time,
    Severity,
    Message
};

static constexpr const u32 MESSAGE_BUFFER_SIZE = 256;

struct LogDescriptor
{
    constexpr LogDescriptor(const char* category, const char* severity, const u32 severityNum, const char* function, const char* file, int line)
        : team(category)/*, severity(severity), severityNum(severityNum), function(function), file(file), line(line)*/
    {

    }

    ~LogDescriptor() = default;

    constexpr void SetValues(const char* category,const char* severity, const char* function, const char* file, const u32 line) noexcept
    {
        //this->line = line;
        //this->severity = severity;
        //this->function = function;
        //this->file = file;
        //this->team = category;
    }

    void SetMessage(const char* message, ...)
    {
        va_list d = nullptr;
        va_start(d, message);
        vsnprintf(this->message, MESSAGE_BUFFER_SIZE, message, d);
        va_end(d);
    }

    //struct
    //{
    //    u8 time : 7;
    //    u8 severity : 1;
    //} Miscellaneous;

    const char* team;

    char message[MESSAGE_BUFFER_SIZE] = {};
};


/** Creates a logging category, do NOT insert Logger before, just the word (automatically appends Log)! */
#define LOG_ADDCATEGORY(CategoryName) \
    /* Generated log name! */ \
    struct Log##CategoryName : public LogEntry \
	{ \
        static constexpr const char* GetName() { return #CategoryName; }; \
        static constexpr const wchar_t* GetNameW() { return L#CategoryName; }; \
        static constexpr const u32 Count = Math::Count(#CategoryName); \
    };

//LOG_ADDCATEGORY(Temp);

#define LINE _CRT_WIDE(__LINE__)
#define Lize(x) L##x

#define MR_LOG(CategoryName, severity, message, ...) \
    if constexpr (bIsRunningDebugMode || severity != Fatal) \
    {\
        do \
        { \
             LogDescriptor descriptor = { CategoryName::GetName(), #severity, severity, __FUNCTION__, __FILE__, __LINE__ };\
             descriptor.SetMessage(message, __VA_ARGS__);\
             if constexpr (severity != Fatal) \
             { \
                char buffer[1024] = {'\0'};\
                Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Time, &descriptor));\
                Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Category, &descriptor));\
                Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Severity, &descriptor));\
                Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Message, &descriptor));\
             } else\
             { \
                 char buffer[1024] = { '\0' }; \
                 Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Time, &descriptor));\
                 Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Category, &descriptor));\
                 Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Severity, &descriptor));\
                 Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Message, &descriptor));\
                 exit(-1); \
             }\
        } while (0); \
    }

#ifdef MR_DEBUG
#define MR_ASSERT(expression, message, ...) \
    do { \
        if (!(expression)) \
        {\
            LogAssertion assertion = { __FILE__, (u32)__LINE__, #expression }; assertion.SetMessage(message); Logger::Get()->TransmitAssertion(&assertion); \
        }\
    } while (0)

#ifdef _WIN64
#endif // _WIN64
#else
#define MR_ASSERT(...) //* This function does nothing, if you want something, switch to Debug!
#endif // DEBUG

#define CONCATTYPENAME(x) \
    L#x