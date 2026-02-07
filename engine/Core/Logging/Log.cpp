/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Log.h"
#include <Commandlet.h>
#include <Types/String.h>

#define USER
#define MB
#include <Windows/Windows.h>

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