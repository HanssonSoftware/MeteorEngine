/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Resource/MemoryManager.h>
#include <type_traits>
#include <stdint.h>
#include <cstdarg>

#define MERGE(x, y) x##y
#define MERGE2(x) #x


struct LogAssertion
{
    constexpr LogAssertion(const char* inputFile, uint32_t inputLine, const char* statement)
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

    uint32_t assertLineInFile = 0;

    const char* assertLocationInFile = nullptr;

    const char* assertStatement = nullptr;

    char message[bIsRunningDebugMode ? 256 : 128] = { '\0' };
};

enum LogSeverity : short
{
    Log,
    Verbose,
    Warn,
    Error,
    Fatal
};

struct LogDescriptor
{
    constexpr LogDescriptor(const char* category, int severity, const char* function, const char* file, int line)
        : team(category), severity(severity), function(function), file(file), line(line)
    {

    }

    ~LogDescriptor() = default;

    constexpr void SetValues(const char* category, int severity, const char* function, const char* file, const int line) noexcept
    {
        this->line = line;
        this->severity = severity;
        this->function = function;
        this->file = file;
        this->team = category;
    }

    void SetMessage(const char* message, ...)
    {
        va_list d = nullptr;
        va_start(d, message);
        vsnprintf(this->message, bIsRunningDebugMode ? 1024 : 256, message, d);
        va_end(d);

    }

    uint8_t severity;
    uint32_t line;

    const char* team;

#ifdef MR_DEBUG
    const char* function;
    const char* file;
#endif // MR_DEBUG

    char message[bIsRunningDebugMode ? 1024 : 256] = { '\0' };
};


/** Creates a logging category, do NOT insert Logger before, just the word (automatically appends Log)! */
#define LOG_ADDCATEGORY(CategoryName) \
    /* Generated log name! */ \
    struct Log##CategoryName : public LogEntry \
	{ \
        static constexpr const char* GetName() { return #CategoryName; }; \
    }

LOG_ADDCATEGORY(Temp);

#define LINE _CRT_WIDE(__LINE__)
#define Lize(x) L##x

#define MR_LOG(CategoryName, Severity, Message, ...) \
	/*static_assert(!std::is_same<decltype(_exception::retval), const wchar_t*>::value, "Formatting must be either TEXT() or L'Text'"); */\
    static_assert(std::is_base_of<LogEntry, CategoryName>::value, "Category must inherit from LogEntry (Use LOG_ADDCATEGORY() macro)"); \
    if constexpr (bIsRunningDebugMode || Severity != Fatal) \
    {\
        do { \
             LogDescriptor descriptor = { #CategoryName, Severity, __FUNCTION__, __FILE__, __LINE__ };\
             descriptor.SetMessage(Message, __VA_ARGS__); \
                if constexpr (Severity >= Fatal) \
                    Logger::Get()->HandleFatal(&descriptor); \
                else if constexpr (Severity < Fatal && bIsRunningDebugMode)\
                    Logger::Get()->TransmitMessage(&descriptor); \
             \
        } while (0); \
    }

#ifdef MR_DEBUG
#define MR_ASSERT(expression, message, ...) \
    do { \
        if (!(expression)) \
        {\
            LogAssertion assertion = { __FILE__, (uint32_t)__LINE__, #expression }; assertion.SetMessage(message); Logger::Get()->TransmitAssertion(&assertion); \
        }\
    } while (0)

#ifdef _WIN64
#endif // _WIN64
#else
#define MR_ASSERT(...) //* This function does nothing, if you want something, switch to Debug!
#endif // DEBUG




#define CONCATTYPENAME(x) \
    L#x