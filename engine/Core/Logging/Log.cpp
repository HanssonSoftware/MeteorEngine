/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Log.h"
#include <Commandline.h>
#include <Types/String.h>
#include <Memory/MemoryBlockArena.h>

#include <Application/Application.h>
#include <Special/EngineConstants.h>
//#include <wchar.h>

#ifdef MR_PLATFORM_WINDOWS
#include "Win32/MinimalWin.h"


static HANDLE consoleHandle;
static HANDLE fileHandle;
#pragma warning(disable : 6386)
#endif // MR_PLATFORM_WINDOWS

LOG_ADDCATEGORY(Logging);
LOG_ADDCATEGORY(LoggingIO);

Logger* Logger::Get()
{
    if (!instance)
    {
        PrepareLoggingSystem();
    }

    return instance;
}

Logger::Logger(Logger* newInstance)
{
    instance = newInstance;
}

Logger::~Logger() noexcept
{
    Shutdown();
}

void Logger::LogStandard(LogEntry* category, LogSeverity severity, const char* message, ...)
{
    if constexpr (true)
    {

    }
}

void Logger::LogFatal(LogEntry* category, LogSeverity severity, const char* message, const u64 time, const char* function, const u32 line, const char* file, ...)
{

}

bool Logger::PrepareLoggingSystem()
{
    MemoryBlockArena* logArena = (MemoryBlockArena*)GetMemoryManager()->RequestNewRegion<MemoryBlockArena>("Logger Region", MAX_LOG_ENTRIES * sizeof(LogEntry) + sizeof(Logger));

    if (logArena)
    {
        instance = (Logger*)logArena;
        instance->loggingArena = logArena;
        return true;
    }

    return false;
}

void Logger::LogAssert()
{
#ifdef MR_PLATFORM_WINDOWS
    //static constexpr const wchar_t title[] = WIDE_ENGINE_NAME_SPACE L" - Assertion error";
    //wchar_t fixed[bIsRunningDebugMode ? 256 : 128] = { L'\0' };
    //MultiByteToWideChar(CP_UTF8, 0, Info->message, bIsRunningDebugMode ? 256 : 128, fixed, bIsRunningDebugMode ? 256 : 128);

    //MessageBoxW(nullptr, fixed, title, MB_OK);
#endif // MR_PLATFORM_WINDOWS

    //char hitMessageBuffer[bIsRunningDebugMode ? 2048 : 1024] = { '\0' };

    //const u32 result = (u32)snprintf(hitMessageBuffer, bIsRunningDebugMode ? 2048 : 1024, "*** AN ASSERT WAS HIT! *** [%s][%s:%u]\n", Info->assertStatement, Info->assertLocationInFile, Info->assertLineInFile);
   
    //SendToOutputBuffer(hitMessageBuffer, result);
}
//
//void Logger::SendToOutputBuffer(char* buffer, const u32 count)
//{
//#if defined(MR_PLATFORM_WINDOWS) && defined(MR_DEBUG)
//
//    bool bShouldUseAllocator = count > 256 ? true : false;
//    wchar_t fixedBuffer[256 + 1] = { L'\0' };
//    
//    wchar_t* fixBuffer = bShouldUseAllocator ? (wchar_t*)GetMemoryManager()->Allocate((count + 1) * sizeof(wchar_t), GetMemoryManager()->GetProjectRegion()) : fixedBuffer;
//
//    if (!MultiByteToWideChar(CP_UTF8, 0, buffer, count * sizeof(char), fixBuffer, count))
//    {
//        MR_ASSERT(false, "fs");
//    }
//
//    OutputDebugStringW(fixBuffer);
//
//    if (consoleHandle)
//    {
//        DWORD written = 0;
//        WriteConsoleW(consoleHandle, (wchar_t*)fixBuffer, (DWORD)count, &written, nullptr);
//    }   
//
//    static bool bFileHandlingState = true;
//    if (fileHandle && bFileHandlingState)
//    {
//        static u32 bytes = 0;
//        DWORD written = 0;
//
//        if (!WriteFile(fileHandle, buffer, count, &written, nullptr))
//        {
//            bFileHandlingState = false;
//
//            CloseHandle(fileHandle);
//            return;
//        }
//
//        bytes += (u32)written;
//        if (bytes > 512)
//        {
//            if (FlushFileBuffers(fileHandle))
//                bytes = 0;
//        }
//    }
//
//    //if (bShouldUseAllocator) GetMemoryManager()->Deallocate(fixBuffer, (count + 1) * sizeof(wchar_t));
//#endif // MR_PLATFORM_WINDOWS && MR_DEBUG
//
//}
