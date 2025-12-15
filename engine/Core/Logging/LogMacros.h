/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <MemoryManager.h>
#include <type_traits>

#define MERGE(x, y) x##y
#define MERGE2(x) #x


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

    ~LogDescriptor()
    {
        if (message != nullptr)
        {
            MemoryManager::Get().Deallocate<char>(message);
        }
    }

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
        const int reqAmount = vsnprintf(nullptr, 0, message, d);

        this->message = MemoryManager::Get().Allocate<char>(reqAmount + 1);
        memset(this->message, 0, reqAmount + 1);

        vsnprintf(this->message, reqAmount + 1, message, d);
        va_end(d);

        this->message[reqAmount] = '\0';
    }

    unsigned int line;

    short severity;

    char* message = nullptr;

    const char* team;

    const char* function;

    const char* file;
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
        {   \
            /*LogAssertion asserta(__FILE__, __LINE__, #expression, message, __VA_ARGS__);*/\
            if (/*Logger::Get()->TransmitAssertion(&asserta) == 1*/1) __debugbreak(); \
                                                             \
        }                                                                      \
    } while (0)

#ifdef _WIN64
#endif // _WIN64
#else
#define MR_ASSERT(...) //* This function does nothing, if you want someting, switch to Debug!
#endif // DEBUG




#define CONCATTYPENAME(x) \
    L#x