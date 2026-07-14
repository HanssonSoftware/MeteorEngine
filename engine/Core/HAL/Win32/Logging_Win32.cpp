/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include <Logging/Log.h>
#include <Application/Application.h>
#include <HAL/Timer.h>
#include <Special/EngineConstants.h>

#include <Win32/MinimalWin.h>
#include <debugapi.h>
#include <Shlobj.h>
#include <shlwapi.h>
#include <pathcch.h>
#include <strsafe.h>

#pragma comment(lib, "OneCore.Lib")
#pragma comment(lib, "Pathcch.lib")

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

            wchar_t* appName = bIsUsingHeap ? (wchar_t*)GetMemoryManager()->Allocate((appNameLength + 1) * sizeof(wchar_t)) : appNameStack;

            if (!HAL::ConvertToWide(appName, appNameLength, GetApplication()->GetApplicationNameNoSpaces()->Chr()))
            {
                MR_LOG(LogInit, Error, "Conversion error occoured!");

                CoTaskMemFree(foundPath);
                GetMemoryManager()->Deallocate(0);
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
            if (persistentLog == INVALID_HANDLE_VALUE)
            {
                wchar_t chars[256] = { L'\0' };

                FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), LANG_USER_DEFAULT, chars, 256, nullptr);

                MR_LOG(LogInit, Error, "%ls", chars);
            }

            constexpr const char fileBeginFormatting[] = "Logging started at: %02d/%02d/%02d %02d:%02d:%02d\n";
            char fileBeginFormatted[64] = {};

            const u32 count = snprintf(fileBeginFormatted, 64, fileBeginFormatting, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

            DWORD toWrite = 0;
            if (!WriteFile(persistentLog, fileBeginFormatted, (DWORD)count, &toWrite, nullptr))
            {
                int J = GetLastError();

                int h = GetLastError();
            }

            CoTaskMemFree(foundPath);
        }
    }

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

void Logger::LogStandard(const u16& category, LogSeverity severity, const void* message, ...)
{
    va_list va;
    va_start(va, message);

    const wchar_t* casted = (wchar_t*)message;

    constexpr const u32 MAX_CHARS_FOR_COMPLETE_MESSAGE = 1024;

    wchar_t buf[MAX_CHARS_FOR_COMPLETE_MESSAGE] = {};
    u32 formattedMessagePos = 0, fullFormattedPos = 0;

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
}

void Logger::LogFatal(const u16& category, LogSeverity severity, const void* message, const void* function, const u32 line, const void* file, ...)
{
    va_list va;
    va_start(va, message);

    const wchar_t* casted = (wchar_t*)message;

    constexpr const u32 MAX_CHARS_FOR_COMPLETE_MESSAGE = 1024;

    wchar_t buf[MAX_CHARS_FOR_COMPLETE_MESSAGE] = {};
    u32 formattedMessagePos = 0, fullFormattedPos = 0;

    if (*casted != L'\0')
    {
        formattedMessagePos += vswprintf_s(buf, MAX_CHARS_FOR_COMPLETE_MESSAGE / 2, casted, va);
    }

    va_end(va);

    SYSTEMTIME st = {};
    GetLocalTime(&st);

    fullFormattedPos = swprintf_s(buf + formattedMessagePos, MAX_CHARS_FOR_COMPLETE_MESSAGE - formattedMessagePos, L"---------- FATAL ERROR ----------\nFunction:%s%02d/%02d/%02d %02d:%02d:%02d][%hs][%hs",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
        buf);

    SendToOutputBuffer(buf + formattedMessagePos, fullFormattedPos, severity);
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