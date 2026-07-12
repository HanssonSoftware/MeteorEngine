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

#pragma comment(lib, "OneCore.Lib")
#pragma comment(lib, "Pathcch.lib")

static HANDLE presistentLog;
static HANDLE consoleLog;

void Logger::Init()
{
    Timer loggingInitializationMeasurement;
    loggingInitializationMeasurement.Start();

    Application* application = GetApplication();
    Commandline* commandline = application->GetCommandline();

    if (!commandline->Check("-nofilelogging"))
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
                MR_LOG(LogLoggingInitialization, Error, "Conversion error occoured!");

                CoTaskMemFree(foundPath);
                GetMemoryManager()->Deallocate(0);
                return;
            }

            wcsncat(path, appName, appNameLength);
            //SHCreateDirectoryExW(nullptr, path, nullptr);


            wcscat(path, L"\\" L"Logs" L"\\");
            SHCreateDirectoryExW(nullptr, path, nullptr);

            SYSTEMTIME st = {};
            GetLocalTime(&st);

            wchar_t date[32] = {};
            swprintf(date, 32, L"%02d%02d%02d-%02d%02d%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
            wcsncat(path, date, 32);

            presistentLog = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING, nullptr);
            if (presistentLog == INVALID_HANDLE_VALUE)
            {
                wchar_t chars[256] = { L'\0' };

                FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), LANG_USER_DEFAULT, chars, 256, nullptr);

                MR_LOG(LogTemp, Error, "%ls", chars);
            }

            constexpr const char fileBeginFormatting[] = "Logging started at: %02d/%02d/%02d %02d:%02d:%02d\n";
            char fileBeginFormatted[64] = "\0";

            const u32 count = snprintf(fileBeginFormatted, 64, fileBeginFormatting, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

            DWORD toWrite = 0;
            WriteFile(presistentLog, fileBeginFormatted, count, &toWrite, nullptr);
            FlushFileBuffers(presistentLog);

            CoTaskMemFree(foundPath);
        }
    }

    if (!GetConsoleWindow())
    {
        //FreeConsole();

        //if (!AllocConsole())
        //{
        //    if (GetLastError() == ERROR_ACCESS_DENIED)
        //    {
        //        if (!AttachConsole(ATTACH_PARENT_PROCESS))
        //        {
        //            GetApplication()->RequestExit(-1);
        //        }
        //    }
        //    else
        //    {
        //        GetApplication()->RequestExit(-1);
        //    }
        //}
        //else
        //{
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

    
    MR_LOG(LogLogging, Log, "Logger system is instantiated in %.3f seconds!", loggingInitializationMeasurement.Count(Timer::CalculateBy::Seconds));
}

void Logger::Shutdown()
{

}

void Logger::SendToOutputBuffer(void* buffer, const u32 count)
{
	if (wchar_t* casted = (wchar_t*)buffer)
	{
		if (IsDebuggerPresent())
		{
			OutputDebugStringW(casted);
		}
	}
}

#endif // MR_PLATFORM_WINDOWS