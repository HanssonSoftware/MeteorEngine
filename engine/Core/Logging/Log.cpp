/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#define _CRT_SECURE_NO_WARNINGS
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
static HANDLE instanceFile;
static HANDLE fileHandle;

static bool bIsDebuggerAttached = false;
#endif // MR_PLATFORM_WINDOWS


Logger::Logger(Logger* newInstance)
{
    instance = newInstance;
}

Logger::~Logger() noexcept
{

}


void Logger::Shutdown()
{
    CloseHandle(instanceFile);
    CloseHandle(fileHandle);
    CloseHandle(consoleHandle);

    bIsInitialized = false;
}

void Logger::Initialize()
{
#ifdef MR_PLATFORM_WINDOWS
    if (!Commandlet::Get().Check(TEXT("-nofilelogging")))
    {
        wchar_t* foundPath = nullptr;

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &foundPath)))
        {
            wchar_t path[512] = { L'\0' };
            wcscpy(path, foundPath);

            static constexpr const wchar_t pathToCat[] = L"\\" WIDE_COMPANY_NAME L"\\";
            wcscat(path, pathToCat);

            wcsncat(path, GetApplication()->GetApplicationNameNoSpaces(), GetApplication()->GetApplicationNameNoSpaces().Length());

            SHCreateDirectoryExW(nullptr, path, nullptr);

            static constexpr const wchar_t instanceRunningFilePath[] = L"\\.livestatus";
            wcsncat(path, instanceRunningFilePath, sizeof(instanceRunningFilePath) / sizeof(instanceRunningFilePath[0]));

            instanceFile = CreateFileW(path, GENERIC_READ, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
            if (GetLastError() == ERROR_ALREADY_EXISTS)
            {
                static constexpr const wchar_t title[] = WIDE_ENGINE_NAME_SPACE L" - Info";
                MessageBoxW(nullptr, L"This application does not shutdown correctly, enable safe mode?", title, MB_OKCANCEL | MB_ICONEXCLAMATION);
            }

            PathCchRemoveFileSpec(path, wcslen(path));

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

                //SetConsoleCP(CP_UTF8);

                CONSOLE_CURSOR_INFO cf;
                GetConsoleCursorInfo(consoleHandle, &cf);
                cf.bVisible = false;

                SetConsoleCursorInfo(consoleHandle, &cf);


                if (!SetStdHandle(STD_OUTPUT_HANDLE, consoleHandle))
                    Application::RequestExit(-1);

                wchar_t title[256] = { L'\0' };

                swprintf(title, 256, L"%ls developer console", GetApplication()->GetApplicationName().Chr());
                SetConsoleTitleW(title);
            }
        }
    }
    //SetUnhandledExceptionFilter(ExceptionHandler);

#endif // MR_PLATFORM_WINDOWS
}

void Logger::HandleFatal(LogDescriptor* Descriptor)
{
    
}

u32 Logger::FormatLogMessage(Char* buffer, LogFormatting format, LogDescriptor* descriptor)
{
    switch (format)
    {
        case LogFormatting::Category:
        {
#ifdef MR_PLATFORM_WINDOWS
            return (u32)swprintf(buffer, 64, L"[%ls] ", descriptor->team);
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

            return (u32)swprintf(buffer, 32, L"[%02u-%02u-%02u %02u:%02u:%02u]", (u16)st.wYear, (u16)st.wMonth, (u16)st.wDay, (u16)st.wHour, (u16)st.wMinute, (u16)st.wSecond);
#endif // MR_PLATFORM_WINDOWS
        }
        case LogFormatting::Severity:
        {
#ifdef MR_PLATFORM_WINDOWS
            return (u32)swprintf(buffer, 8, L"%ls: \0", descriptor->severity);
#endif // MR_PLATFORM_WINDOWS
        }
        case LogFormatting::Message:
        {
#ifdef MR_PLATFORM_WINDOWS
            static constexpr const Char val[] = TEXT("\n\0");
            return (u32)swprintf(buffer, (bIsRunningDebugMode ? 512 : 128) + 3, L"%ls\n\0", descriptor->message);
#endif // MR_PLATFORM_WINDOWS
        }
    }

    return (u32)0;
}

void Logger::TransmitAssertion(const LogAssertion* Info)
{
	if (!Info) return;

#ifdef MR_PLATFORM_WINDOWS
    MessageBoxA(nullptr, Info->message, "Assertion failed!", MB_ICONEXCLAMATION | MB_OK);
#endif // MR_PLATFORM_WINDOWS
}

void Logger::SendToOutputBuffer(Char* buffer, const u32 count)
{
#if defined(MR_PLATFORM_WINDOWS) && defined(MR_DEBUG)

    if (bIsDebuggerAttached)
    {
        OutputDebugStringW(buffer);
    }
    else if (consoleHandle)
    {
        DWORD written = 0;
        WriteConsoleW(consoleHandle, (Char*)buffer, (DWORD)count, &written, nullptr);
    }
    else
    {
        // ...
    }     

    if (fileHandle)
    {
        char fixBuffer[bIsRunningDebugMode ? 512 : 128] = {};

        static u32 bytes = 0;
        DWORD written = 0;

        WideCharToMultiByte(CP_UTF8, 0, buffer, count, fixBuffer, count * sizeof(char), nullptr, nullptr);

        if (!WriteFile(fileHandle, fixBuffer, count, &written, nullptr))
            return;

        bytes += (u32)written;
        if (bytes > 512)
        {
            if (FlushFileBuffers(fileHandle))
                bytes = 0;
        }
    }

    wmemset(buffer, L'\0', count);

#else
    memset(buffer, 0, count);
#endif // MR_PLATFORM_WINDOWS && MR_DEBUG

}