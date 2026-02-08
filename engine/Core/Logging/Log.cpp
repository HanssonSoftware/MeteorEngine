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

#pragma comment(lib, "OneCore.Lib")

Logger::Logger(Logger* newInstance)
{
    instance = newInstance;
}

void Logger::Shutdown()
{
    bIsInitialized = false;
}

void Logger::Initialize()
{
#ifdef MR_PLATFORM_WINDOWS
    wchar_t* foundPath = nullptr;

    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &foundPath)))
    {
        wchar_t path[512] = { L'\0' };
        wcscpy(path, foundPath);

        static constexpr const wchar_t pathToCat[] = L"\\" WIDE_COMPANY_NAME L"\\";
        wcscat(path, pathToCat);

        wchar_t path2[64] = { L'\0' };
        const uint32_t converted = MultiByteToWideChar(CP_UTF8, 0, GetApplication()->GetApplicationNameNoSpaces(), GetApplication()->GetApplicationNameNoSpaces().Length(), path2, GetApplication()->GetApplicationNameNoSpaces().Length());
        if (!converted)
        {

        }

        wcsncat(path, path2, converted);

        static constexpr const wchar_t pathToCat2[] = L"\\" L"Logs" L"\\";
        wcsncat(path, pathToCat2, 7);

        SYSTEMTIME st = {};
        GetLocalTime(&st);

        wchar_t date[32] = { L'\0' };
        swprintf(date, 32, L"%02d-%02d-%02d %02d:%02d:%02d.txt", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        wcsncat(path, date, 32);

        fileHandle = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (fileHandle == INVALID_HANDLE_VALUE)
        {
            wchar_t chars[256] = { L'\0' };

            FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), LANG_USER_DEFAULT, chars, 256, nullptr);

            MR_LOG(LogTemp, Error, "%ls", chars);
        }

        CoTaskMemFree(foundPath);
    }
    //bIsUsingVerbose = Commandlet::Get().Parse("-verbose", nullptr);

    //bIsUsingFile = !Commandlet::Get().Parse("-nofilelogging", nullptr);

    //if (bIsUsingFile)
    //{
    //    auto b = Paths::GetExecutableDirctory();
    //    //buffer = FileManager::CreateFileOperation(
    //    //    String::Format(
    //    //        "E:\\Logger\\%ls-%ls.txt", 
    //    //        app->GetAppInfo().appName.Chr(), 
    //    //        ITimer::Now("%H.%M.%ls").Chr()),

    //    //    OPENMODE_WRITE | OPENMODE_READ, SHAREMODE_READ | SHAREMODE_WRITE, OVERRIDERULE_CREATE_NEW_DONT_MIND, 
    //    //    stat
    //    //);

    //    if (buffer) buffer->Write(nullptr);
    //}
#endif // MR_PLATFORM_WINDOWS
}

Logger::~Logger() noexcept
{
    Shutdown();
}

void Logger::HandleFatal(LogDescriptor* Descriptor)
{
    exit(0);
}

void Logger::TransmitMessage(LogDescriptor* Descriptor)
{
    String fullMessage;
    const String current = /*Timer::Format()*/"yyyy-MM-dd HH:mm:ss";

    if (Descriptor->severity == Fatal)
    {
        fullMessage = String::Format(
            "=============[ Fatal error ]=============\nWhere:\t\t%s\nWhen:\t\t%s\nMessage:\t%s\n\nFile:\t%s\n",
            Descriptor->function,
            *current,
            Descriptor->message,
            Descriptor->file);
    }
    else
    {
        fullMessage = String::Format("[%s] %s: %s\n", 
            current.Chr(),
            Descriptor->team,
            Descriptor->message);
    }

    SendToOutputBuffer(&fullMessage);
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
    OutputDebugStringA(*Buffer);
    //OutputDebugStringA("\n");
#endif // MR_PLATFORM_WINDOWS && MR_DEBUG

}