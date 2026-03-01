/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Log.h"
#include <Commandlet.h>
#include <Types/String.h>


#include <Application/Application.h>
#include <Special/EngineConstants.h>
#include <wchar.h>

#define USER
#define MB
#include <Windows/Windows.h>
#include <Shlobj.h>
#include <shlwapi.h>
#include <pathcch.h>

#pragma comment(lib, "OneCore.Lib")
#pragma comment(lib, "Pathcch.lib")

#ifdef MR_PLATFORM_WINDOWS
static HANDLE consoleHandle;
static HANDLE fileHandle;

static bool bIsDebuggerAttached = false;
#pragma warning(disable : 6386)
#endif // MR_PLATFORM_WINDOWS

LOG_ADDCATEGORY(ging); // Log(ging)

static Logger* instance = nullptr;

Logger* Logger::Get()
{
    if (!instance)
    {
        instance = new Logger;
    }

    return instance;
}

Logger::Logger(Logger* newInstance)
{
    instance = newInstance;
}

Logger::~Logger() noexcept
{

}


#ifdef MR_PLATFORM_WINDOWS
static HANDLE thread;
volatile bool bRunMemoryLogger = false;
static DWORD WINAPI MemoryLoggingProc(void* lpParameter)
{
    bRunMemoryLogger = true;
    while (bRunMemoryLogger)
    {
        Sleep(30 * 1000);

        _CrtDumpMemoryLeaks();
    }

    return 0;
}
#endif // MR_PLATFORM_WINDOWS

void Logger::Shutdown()
{
    CloseHandle(fileHandle);
    CloseHandle(consoleHandle);

    bRunMemoryLogger = false;

    bIsInitialized = false;
}

void Logger::Initialize()
{
#ifdef MR_PLATFORM_WINDOWS
    LARGE_INTEGER begin, freq, end;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&begin);

    if (!Commandlet::Get().Check("-nofilelogging"))
    {
        wchar_t* foundPath = nullptr;

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &foundPath)))
        {
            wchar_t path[512 + 1] = { L'\0' };
            wcscpy(path, foundPath);

            static constexpr const wchar_t pathToCat[] = L"\\" WIDE_COMPANY_NAME L"\\";
            wcscat(path, pathToCat);

            const u32 appNameLength = (u32)strlen(GetApplication()->GetApplicationNameNoSpaces());
            const bool bIsUsingHeap = appNameLength > 256 ? true : false;
            wchar_t appNameStack[256 + 1] = { L'\0' };

            wchar_t* appName = bIsUsingHeap ? (wchar_t*)GetMemoryManager()->Allocate((appNameLength + 1) * sizeof(wchar_t)) : appNameStack;

            if (!MultiByteToWideChar(CP_UTF8, 0, GetApplication()->GetApplicationNameNoSpaces(), appNameLength, appName, appNameLength))
            {
                wchar_t chars[256] = { L'\0' };

                chars[FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), LANG_USER_DEFAULT, chars, 256, nullptr) - 2] = L'\0';

                MR_LOG(LogTemp, Error, "%ls", chars);
            }

            wcsncat(path, appName, appNameLength);

            SHCreateDirectoryExW(nullptr, path, nullptr);

            static constexpr const wchar_t pathToCat2[] = L"\\" L"Logs" L"\\";
            wcsncat(path, pathToCat2, sizeof(pathToCat2) / sizeof(pathToCat2[0]));
            SHCreateDirectoryExW(nullptr, path, nullptr);

            SYSTEMTIME st = {};
            GetLocalTime(&st);

            wchar_t date[32] = { L'\0' };
            swprintf(date, 32, L"%02d%02d%02d-%02d%02d%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
            wcsncat(path, date, 32);

            fileHandle = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
            if (fileHandle == INVALID_HANDLE_VALUE)
            {
                wchar_t chars[256] = { L'\0' };

                FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), LANG_USER_DEFAULT, chars, 256, nullptr);

                MR_LOG(LogTemp, Error, "%ls", chars);
            }

            static constexpr const char fileBeginFormatting[] = "Logging started at: %02d/%02d/%02d %02d:%02d:%02d\n";
            char fileBeginFormatted[64] = "\0";

            const u32 count = snprintf(fileBeginFormatted, 64, fileBeginFormatting, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

            DWORD toWrite = 0;
            WriteFile(fileHandle, fileBeginFormatted, count, &toWrite, nullptr);
            FlushFileBuffers(fileHandle);

            CoTaskMemFree(foundPath);
        }
    }
    
    bIsDebuggerAttached = IsDebuggerPresent() ? true : false;
    if (!bIsDebuggerAttached)
    {
        FreeConsole();

        if (!AllocConsole())
        {
            if (GetLastError() == ERROR_ACCESS_DENIED)
            {
                if (!AttachConsole(ATTACH_PARENT_PROCESS))
                {
                    Application::RequestExit(-1);
                }
            }
            else
            {
                Application::RequestExit(-1);
            }
        }
        else
        {
            if (!SetStdHandle(STD_INPUT_HANDLE, INVALID_HANDLE_VALUE))
            {
                Application::RequestExit(-1);
            }

            consoleHandle = CreateFileW(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
            if (consoleHandle != INVALID_HANDLE_VALUE)
            {
                SetConsoleOutputCP(CP_UTF8);

                CONSOLE_CURSOR_INFO cf;
                GetConsoleCursorInfo(consoleHandle, &cf);
                cf.bVisible = false;

                SetConsoleCursorInfo(consoleHandle, &cf);


                if (!SetStdHandle(STD_OUTPUT_HANDLE, consoleHandle))
                    Application::RequestExit(-1);

                wchar_t title[256] = { L'\0' };

                swprintf(title, 256, L"%ls developer console", L"FILL THIS SWPRINTF PARAMETER!!!");
                SetConsoleTitleW(title);
            }
        }
    }

    QueryPerformanceCounter(&end);
    MR_LOG(Logging, Log, "Logger system is instantiated in %.2f seconds!", (end.QuadPart - begin.QuadPart) / (freq.QuadPart / 100.0));
#endif // MR_PLATFORM_WINDOWS

    StartMemoryLeakLoggingThread();
}

void Logger::HandleFatal(LogDescriptor* Descriptor)
{
    
}

void Logger::StartMemoryLeakLoggingThread()
{
#ifdef MR_PLATFORM_WINDOWS
    thread = CreateThread(nullptr, 512 * 1024, MemoryLoggingProc, nullptr, 0, nullptr);
#endif // MR_PLATFORM_WINDOWS
}

u32 Logger::FormatLogMessage(char* buffer, LogFormatting format, LogDescriptor* descriptor)
{
    switch (format)
    {
        case LogFormatting::Category:
        {
#ifdef MR_PLATFORM_WINDOWS
            return (u32)snprintf(buffer, 64, "[%s] ", descriptor->team);
#endif // MR_PLATFORM_WINDOWS
        }
        case LogFormatting::Time:
        {
#ifdef MR_PLATFORM_WINDOWS
            SYSTEMTIME st = {};
            GetLocalTime(&st);

            if (consoleHandle)
            {
                switch (descriptor->severityNum)
                {
                    case 1: SetConsoleTextAttribute(consoleHandle, 0xf); break;
                    case 2: SetConsoleTextAttribute(consoleHandle, 0x9); break;
                    case 3: SetConsoleTextAttribute(consoleHandle, 0x6); break;
                    case 4: SetConsoleTextAttribute(consoleHandle, 0x4); break;
                    case 5: SetConsoleTextAttribute(consoleHandle, 0x4); break;
                }
            }

            return (u32)snprintf(buffer, 32, "[%02u-%02u-%02u %02u:%02u:%02u]", (u16)st.wYear, (u16)st.wMonth, (u16)st.wDay, (u16)st.wHour, (u16)st.wMinute, (u16)st.wSecond);
#endif // MR_PLATFORM_WINDOWS
        }
        case LogFormatting::Severity:
        {
#ifdef MR_PLATFORM_WINDOWS
            return (u32)snprintf(buffer, 8, "%s: \0", descriptor->severity);
#endif // MR_PLATFORM_WINDOWS
        }
        case LogFormatting::Message:
        {
#ifdef MR_PLATFORM_WINDOWS
            static constexpr const char val[] = "\n\0";
            return (u32)snprintf(buffer, (bIsRunningDebugMode ? 512 : 128) + 3, "%s\n\0", descriptor->message);
#endif // MR_PLATFORM_WINDOWS
        }
    }

    return (u32)0;
}

void Logger::TransmitAssertion(const LogAssertion* Info)
{
#ifdef MR_PLATFORM_WINDOWS
    static constexpr const wchar_t title[] = WIDE_ENGINE_NAME_SPACE L" - Assertion error";
    wchar_t fixed[bIsRunningDebugMode ? 256 : 128] = { L'\0' };
    MultiByteToWideChar(CP_UTF8, 0, Info->message, bIsRunningDebugMode ? 256 : 128, fixed, bIsRunningDebugMode ? 256 : 128);

    MessageBoxW(nullptr, fixed, title, MB_OK);
#endif // MR_PLATFORM_WINDOWS

    char hitMessageBuffer[bIsRunningDebugMode ? 2048 : 1024] = { '\0' };

    const u32 result = (u32)snprintf(hitMessageBuffer, bIsRunningDebugMode ? 2048 : 1024, "*** AN ASSERT WAS HIT! *** [%s][%s:%u]\n", Info->assertStatement, Info->assertLocationInFile, Info->assertLineInFile);
   
    SendToOutputBuffer(hitMessageBuffer, result);

#ifdef MR_PLATFORM_WINDOWS
    DebugBreak();
#endif
}

void Logger::SendToOutputBuffer(char* buffer, const u32 count)
{
#if defined(MR_PLATFORM_WINDOWS) && defined(MR_DEBUG)

    bool bShouldUseAllocator = count > 256 ? true : false;
    wchar_t fixedBuffer[256 + 1] = { L'\0' };
    
    wchar_t* fixBuffer = bShouldUseAllocator ? (wchar_t*)GetMemoryManager()->Allocate((count + 1) * sizeof(wchar_t)) : fixedBuffer;

    if (!MultiByteToWideChar(CP_UTF8, 0, buffer, count * sizeof(char), fixBuffer, count))
    {
        MR_ASSERT(false, "fs");
    }

    if (bIsDebuggerAttached)
    {
        OutputDebugStringW(fixBuffer);
    }
    else if (consoleHandle)
    {
        DWORD written = 0;
        WriteConsoleW(consoleHandle, (wchar_t*)fixBuffer, (DWORD)count, &written, nullptr);
    }
    else
    {
        // ...
    }     

    if (fileHandle)
    {
        static u32 bytes = 0;
        DWORD written = 0;

        if (!WriteFile(fileHandle, buffer, count, &written, nullptr))
            return;

        bytes += (u32)written;
        if (bytes > 512)
        {
            if (FlushFileBuffers(fileHandle))
                bytes = 0;
        }
    }

    if (bShouldUseAllocator) GetMemoryManager()->Deallocate(fixBuffer, (count + 1) * sizeof(wchar_t));
#endif // MR_PLATFORM_WINDOWS && MR_DEBUG

}