/* Copyright 2020 = 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include <Logging/Log.h>
#include <Memory/MemoryHandler.h>
#include <Application/Application.h>
#include <HAL/Timer.h>
#include <Special/EngineConstants.h>
#include <HAL/Window.h>

#include <Win32/MinimalWin.h>
#include <debugapi.h>
#include <Shlobj.h>
#include <shlwapi.h>
#include <pathcch.h>
#include <strsafe.h>

#include <DefaultEngineResources.h>

#pragma comment(lib, "OneCore.Lib")
#pragma comment(lib, "Pathcch.lib")
#pragma warning(disable : 6053) // The prior call to 'wcsncpy' might not zero-terminate string 'messageBox'.

static HANDLE persistentLog;
static HANDLE consoleLog;

static constexpr const WORD ConvertToColorCode(LogSeverity severity);

LOG_ADDCATEGORY(Init);

void Logger::Init()
{
    Timer loggingInitializationMeasurement;
    loggingInitializationMeasurement.Start();

    Application* application = GetApplication();
    Commandline* commandline = application->GetCommandline();

    if (!commandline->Check("-nofilelogging") && false) // TODO: Remove false
    {
        wchar_t* foundPath = nullptr;

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &foundPath)))
        {
            wchar_t path[2048] = {};
            wcscpy(path, foundPath);

            wcscat(path, L"\\" WIDE_COMPANY_NAME L"\\");

            const u32 appNameLength = GetApplication()->GetApplicationNameNoSpaces()->Length();
            const bool bIsUsingHeap = appNameLength > 255 ? true : false;
            wchar_t appNameStack[256] = {};

            wchar_t* appName = bIsUsingHeap ? GetMemoryManager()->Allocate<wchar_t>((appNameLength + 1) * sizeof(wchar_t)) : appNameStack;

            if (!HAL::ConvertToWide(appName, appNameLength, GetApplication()->GetApplicationNameNoSpaces()->Chr()))
            {
                MR_LOG(LogInit, Error, "Conversion error occoured!");

                CoTaskMemFree(foundPath);
                GetMemoryManager()->Deallocate(nullptr);
                return;
            }

            wcsncat(path, appName, appNameLength);

            wcscat(path, L"\\" L"logs" L"\\");
            SHCreateDirectoryExW(nullptr, path, nullptr);

            SYSTEMTIME st = {};
            GetLocalTime(&st);

            wchar_t date[32] = {};
            swprintf(date, 32, L"%02d%02d%02d-%02d%02d%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
            wcsncat(path, date, 32);

            persistentLog = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING, nullptr);
            if (persistentLog != INVALID_HANDLE_VALUE)
            {
                constexpr const char fileBeginFormatting[] = "Logging started at: %02d/%02d/%02d %02d:%02d:%02d\n";
                char fileBeginFormatted[64] = {};

                const u32 count = snprintf(fileBeginFormatted, 64, fileBeginFormatting, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

                DWORD toWrite = 0;
                if (!WriteFile(persistentLog, fileBeginFormatted, (DWORD)count, &toWrite, nullptr))
                {
                    int J = GetLastError();

                    int h = GetLastError();
                }
            }
            else
            {
                wchar_t chars[256] = { L'\0' };

                FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), LANG_USER_DEFAULT, chars, 256, nullptr);

                MR_LOG(LogInit, Error, "%ls", chars);
            }


            CoTaskMemFree(foundPath);
        }
    }

    if constexpr (bIsRunningDebugMode)
    {
        if (!AttachConsole(ATTACH_PARENT_PROCESS))
            AllocConsole();

        HWND consoleWindow = GetConsoleWindow();
        if (consoleWindow)
        {

            consoleLog = CreateFileW(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
            if (consoleLog != INVALID_HANDLE_VALUE)
            {
                SetConsoleCP(CP_UTF8);
                SetConsoleOutputCP(CP_UTF8);

                CONSOLE_CURSOR_INFO cf;
                GetConsoleCursorInfo(consoleLog, &cf);
                cf.bVisible = false;

                SetConsoleCursorInfo(consoleLog, &cf);

                if (!SetStdHandle(STD_OUTPUT_HANDLE, consoleLog))
                    GetApplication()->RequestExit(-1);

                wchar_t title[128] = {};

                swprintf(title, 128, L"%hs - Developer Console", GetApplication()->GetApplicationName()->Chr());
                SetConsoleTitleW(title);

                HICON hIcon = (HICON)LoadImageW(
                    (HINSTANCE)HAL::GetEngineCore(),
                    MAKEINTRESOURCEW(IDI_LASTRESORT),
                    IMAGE_ICON,
                    0, 0,
                    LR_DEFAULTSIZE | LR_SHARED);
                if (hIcon)
                {
                    SendMessageW(consoleWindow, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
                    SendMessageW(consoleWindow, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
                }
            }
        }
    }

    loggingInitializationMeasurement.Stop();

    MR_LOG(LogInit, Log, "Logger system is instantiated in %.3f seconds!", loggingInitializationMeasurement.Count(Timer::CalculateBy::Seconds));
}

void Logger::Shutdown()
{
    if (!GetApplication()->GetCommandline()->Check("-nofilelogging"))
    {
        CloseHandle(persistentLog);
    }

    CloseHandle(consoleLog);
}

static u32 formattedMessagePos = 0, fullFormattedPos = 0;
constexpr const u32 MAX_CHARS_FOR_COMPLETE_MESSAGE = 2048;
constexpr const u32 MAX_CHARS_FOR_COMPLETE_MESSAGE_FATAL = 2048;

void Logger::LogStandard(const u16& category, LogSeverity severity, const void* message, ...)
{
    va_list va;
    va_start(va, message);

    const wchar_t* casted = (wchar_t*)message;

    wchar_t buf[MAX_CHARS_FOR_COMPLETE_MESSAGE] = {};

    if (*casted != L'\0')
    {
        formattedMessagePos += vswprintf_s(buf, MAX_CHARS_FOR_COMPLETE_MESSAGE / 2, casted, va);
    }

    va_end(va);
    
    SYSTEMTIME st = {};
    GetLocalTime(&st);

    fullFormattedPos = swprintf_s(buf + formattedMessagePos, MAX_CHARS_FOR_COMPLETE_MESSAGE - formattedMessagePos, L"[%02d/%02d/%02d %02d:%02d:%02d][%hs][%hs]: %.*s\n",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
        FindLogCategory(category), FormatSeverity(severity), formattedMessagePos, buf);

    SendToOutputBuffer(buf + formattedMessagePos, fullFormattedPos, severity);
    formattedMessagePos = 0, fullFormattedPos = 0;
}

void Logger::LogFatal(const u16& category, LogSeverity severity, const void* message, const void* function, const u32 line, const void* file, ...)
{
    va_list va;
    va_start(va, file);

    const wchar_t* castedMessage = (wchar_t*)message;
    const wchar_t* castedFunction = (wchar_t*)function;
    const wchar_t* castedFile = (wchar_t*)file;

    wchar_t buf[MAX_CHARS_FOR_COMPLETE_MESSAGE_FATAL] = {};

    if (*castedMessage != L'\0')
    {
        formattedMessagePos += vswprintf_s(buf, MAX_CHARS_FOR_COMPLETE_MESSAGE_FATAL / 2, castedMessage, va);
    }

    va_end(va);

    SYSTEMTIME st = {};
    GetLocalTime(&st);

    fullFormattedPos = swprintf_s(buf + formattedMessagePos, MAX_CHARS_FOR_COMPLETE_MESSAGE_FATAL - formattedMessagePos, L"========== FATAL ERROR ==========\nMessage: %.*s\n\nWhere: [%s(%d) - %s]\nTime: %02d/%02d/%02d %02d:%02d:%02d\n",
        formattedMessagePos, buf, castedFile, line, castedFunction,
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond
        );

    SendToOutputBuffer(buf + formattedMessagePos, fullFormattedPos, severity);
    formattedMessagePos = 0, fullFormattedPos = 0;
}

void Logger::SendToOutputBuffer(void* buffer, const u32 count, LogSeverity severity)
{
    if (wchar_t* casted = (wchar_t*)buffer)
    {
        if (count < 0) return;

        if (IsDebuggerPresent())
        {
            OutputDebugStringW(casted);
        }

        if (persistentLog != INVALID_HANDLE_VALUE)
        {
            char narrow[1024] = {};

            WideCharToMultiByte(CP_UTF8, 0, casted, count, narrow, 1024, nullptr, nullptr);

            static DWORD write = 0;

            DWORD current = 0;
            if (!WriteFile(persistentLog, narrow, count, &current, nullptr))
            {

            }

            write += current;

            if (write > 512 || severity == Fatal)
            {
                write = 0;
                FlushFileBuffers(persistentLog);
            }
        }

        if (consoleLog != INVALID_HANDLE_VALUE)
        {
            SetConsoleTextAttribute(consoleLog, ConvertToColorCode(severity));

            DWORD current = 0;
            if (!WriteConsoleW(consoleLog, casted, count, &current, nullptr))
            {

            }

            SetConsoleTextAttribute(consoleLog, 0x7);
        }


        if (severity == Fatal)
        {
#if MR_DEBUG
            wchar_t messageBox[512] = {};
            wcsncpy(messageBox, casted - formattedMessagePos, formattedMessagePos);

            HWND window = GetApplication()->GetMainWindow() ? (HWND)GetApplication()->GetMainWindow()->GetNativeHandle() : nullptr;

            swprintf(messageBox + formattedMessagePos + 1, 512u - formattedMessagePos, L"%hs has crashed!", GetApplication()->GetApplicationName()->Chr());

            MessageBoxW(window ? window : nullptr, messageBox, messageBox + formattedMessagePos + 1, MB_OK | MB_ICONERROR);
#else
            MessageBeep(MB_ICONERROR);
#endif // MR_DEBUG
        }
    }
}

static constexpr const WORD ConvertToColorCode(LogSeverity severity)
{
    switch (severity)
    {
        case LogSeverity::Log:
            return 0x7;
        case LogSeverity::Warn:
            return 0x6;
        case LogSeverity::Error:
            return 0xC;
        case LogSeverity::Fatal:
            return 0x4;
        case LogSeverity::Verbose:
            return 0x3;
    }

    return 0x7;
}

#endif // MR_PLATFORM_WINDOWS