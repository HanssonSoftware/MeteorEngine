/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Log.h"
#include <Commandline.h>
#include <Types/String.h>
#include <Memory/MemoryBlockArena.h>

#include <Application/Application.h>
#include <Special/EngineConstants.h>
//#include <wchar.h>

static u16 categoryID;
const char* categories[512] = {};

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

bool Logger::PrepareLoggingSystem()
{
    MemoryBlockArena* logArena = (MemoryBlockArena*)GetMemoryManager()->RequestNewRegion<MemoryBlockArena>("Logger Region", MAX_LOG_ENTRIES * sizeof(LogEntry) + sizeof(Logger));

    if (logArena)
    {
        instance = (Logger*)logArena->Allocate(sizeof(Logger));
        logArena->SetMarker(sizeof(Logger));

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

u16 Logger::AddLogCategory(const char* name)
{
    for (u16 i = 0; i < categoryID + 1; i++)
    {
        if (!categories[i]) continue;

        if (!strcmp(categories[i], name))
        {
            return i;
        }
    }

    if (categories[0] != nullptr)
        categoryID++;
    else
        categoryID = 0;

    categories[categoryID] = name;
    return categoryID;
}

const char* Logger::FindLogCategory(const u16& name)
{
    return categories[name];
}

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
