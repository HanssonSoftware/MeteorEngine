/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Resource/MemoryManager.h>
#include <type_traits>
#include <Platform/DataTypes.h>

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

enum LogSeverity
{
    Log      = 1,
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

struct LogDescriptor
{
    constexpr LogDescriptor(const Char* category, const Char* severity, const u32 severityNum, const Char* function, const Char* file, int line)
        : team(category), severity(severity), severityNum(severityNum), function(function), file(file), line(line)
    {

    }

    ~LogDescriptor() = default;

    constexpr void SetValues(const Char* category,const Char* severity, const Char* function, const Char* file, const u32 line) noexcept
    {
        this->line = line;
        this->severity = severity;
        this->function = function;
        this->file = file;
        this->team = category;
    }

    void SetMessage(const Char* message, ...)
    {
        va_list d = nullptr;
        va_start(d, message);
#ifdef MR_PLATFORM_WINDOWS
        vswprintf(this->message, bIsRunningDebugMode ? 512 : 128, message, d);
#else
        vsnprintf(this->message, bIsRunningDebugMode ? 512 : 128, message, d);
#endif
        va_end(d);
    }

    const Char* severity;
    u32 severityNum;

    u32 line;

    const Char* team;

#ifdef MR_DEBUG
    const Char* function;
    const Char* file;
#endif // MR_DEBUG

    Char message[bIsRunningDebugMode ? 512 : 128] = { '\0' };
};


/** Creates a logging category, do NOT insert Logger before, just the word (automatically appends Log)! */
#define LOG_ADDCATEGORY(CategoryName) \
    /* Generated log name! */ \
    struct Log##CategoryName : public LogEntry \
	{ \
        static constexpr const char* GetName() { return #CategoryName; }; \
        static constexpr const wchar_t* GetNameW() { return L#CategoryName; }; \
    }

LOG_ADDCATEGORY(Temp);

#define LINE _CRT_WIDE(__LINE__)
#define Lize(x) L##x

#define MR_LOG(CategoryName, severity, message, ...) \
	/*static_assert(!std::is_same<decltype(_exception::retval), const wchar_t*>::value, "Formatting must be either TEXT() or L'Text'"); */\
    static_assert(std::is_base_of<LogEntry, CategoryName>::value, "Category must inherit from LogEntry (Use LOG_ADDCATEGORY() macro)"); \
    if constexpr (bIsRunningDebugMode || severity != Fatal) \
    {\
        do \
        { \
             LogDescriptor descriptor = { TEXT(#CategoryName), TEXT(#severity), severity, __FUNCTIONW__, __FILEW__, __LINE__ };\
             descriptor.SetMessage(TEXT(message), __VA_ARGS__);\
             if constexpr (severity != Fatal) \
             { \
                Char buffer[1024] = {'\0'};\
                Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Time, &descriptor));\
                Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Category, &descriptor));\
                Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Severity, &descriptor));\
                Logger::Get()->SendToOutputBuffer(buffer, Logger::Get()->FormatLogMessage(buffer, LogFormatting::Message, &descriptor));\
             } \
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