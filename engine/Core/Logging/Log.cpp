/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#define _CRT_SECURE_NO_WARNINGS
#include "Log.h"
#include <Commandlet.h>
#include <Types/String.h>
#include <Platform/DataTypes.h>

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
static HANDLE instanceFile;
static HANDLE fileHandle;
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

    bIsInitialized = false;
}

void Logger::Initialize()
{
#ifdef MR_PLATFORM_WINDOWS
    if (!Commandlet::Get().Check(TEXT("-nofilelogging")))
    {
        Char* foundPath = nullptr;

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &foundPath)))
        {
            Char path[512] = { L'\0' };
            wcscpy(path, foundPath);

            static constexpr const Char pathToCat[] = L"\\" WIDE_COMPANY_NAME L"\\";
            wcscat(path, pathToCat);

            wcsncat(path, GetApplication()->GetApplicationNameNoSpaces(), GetApplication()->GetApplicationNameNoSpaces().Length());

            SHCreateDirectoryExW(nullptr, path, nullptr);

            static constexpr const Char instanceRunningFilePath[] = L"\\.livestatus";
            wcsncat(path, instanceRunningFilePath, sizeof(instanceRunningFilePath) / sizeof(instanceRunningFilePath[0]));

            instanceFile = CreateFileW(path, GENERIC_READ, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
            if (GetLastError() == ERROR_ALREADY_EXISTS)
            {
                static constexpr const Char title[] = WIDE_ENGINE_NAME_SPACE L" - Info";
                MessageBoxW(nullptr, L"This application does not shutdown correctly, enable safe mode?", title, MB_OKCANCEL | MB_ICONEXCLAMATION);
            }

            PathCchRemoveFileSpec(path, wcslen(path));

            static constexpr const Char pathToCat2[] = L"\\" L"Logs" L"\\";
            wcsncat(path, pathToCat2, sizeof(pathToCat2) / sizeof(pathToCat2[0]));
            SHCreateDirectoryExW(nullptr, path, nullptr);

            SYSTEMTIME st = {};
            GetLocalTime(&st);

            Char date[32] = { L'\0' };
            swprintf(date, 32, L"%02d%02d%02d-%02d%02d%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
            wcsncat(path, date, 32);

            fileHandle = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
            if (fileHandle == INVALID_HANDLE_VALUE)
            {
                Char chars[256] = { L'\0' };

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
#endif // MR_PLATFORM_WINDOWS
}

void Logger::HandleFatal(LogDescriptor* Descriptor)
{
    
}

void Logger::FormatLogMessage(LogDescriptor* Descriptor)
{
 
#if 0 /*MR_PLATFORM_WINDOWS*/
    SYSTEMTIME st = {};
    GetLocalTime(&st);
#endif // MR_PLATFORM_WINDOWS

}

void Logger::TransmitAssertion(const LogAssertion* Info)
{
	if (!Info) return;

#ifdef MR_PLATFORM_WINDOWS
    MessageBoxA(nullptr, Info->message, "Assertion failed!", MB_ICONEXCLAMATION | MB_OK);
#endif // MR_PLATFORM_WINDOWS
}

void Logger::SendToOutputBuffer(const String* Buffer)
{
#if defined(MR_PLATFORM_WINDOWS) && defined(MR_DEBUG)
    //OutputDebugStringA(*Buffer);
    
    DWORD written = 0;
    if (!WriteFile(fileHandle, Buffer->Chr(), Buffer->Length(), &written, nullptr))
    {

    }
#endif // MR_PLATFORM_WINDOWS && MR_DEBUG

}