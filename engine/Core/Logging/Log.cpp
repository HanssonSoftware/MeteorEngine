/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Log.h"
#include <Commandlet.h>
#include <MemoryManager.h>

LOG_ADDCATEGORY(Standard);

Logger::Logger()
{
    Initialize();
}

void Logger::Shutdown()
{
    bIsInitialized = false;
}

void Logger::Initialize()
{
    //bIsUsingVerbose = Commandlet::Parse("-verbose", nullptr);

    //bIsUsingFile = !Commandlet::Parse("-nofilelogging", nullptr);

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

}

void Logger::TransmitMessage(LogDescriptor* Descriptor)
{
    current = Descriptor;

    String fullMessage;
    const String current = /*Timer::Format()*/"yyyy-MM-dd HH:mm:ss";

    if (Descriptor->severity == Fatal)
    {
        fullMessage = String::Format(
            "=============[ Fatal error ]=============\nWhere:\t\t%ls\nWhen:\t\t%ls\nMessage:\t%ls\n\nFile:\t%ls\n",
            Descriptor->function,
            *current,
            *Descriptor->message,
            Descriptor->file);
    }
    else
    {
        fullMessage = String::Format("[%ls] %ls: %ls\n", 
            current.Chr(),
            Descriptor->team,
            *Descriptor->message);
    }

    SendToOutputBuffer(&fullMessage);
}

void Logger::TransmitAssertion(const LogAssertion* Info)
{
	//if (!Info) return;

 //   MessageBoxDescriptor mbxInfo = {};
 //   mbxInfo.Description = String::Format(
 //       "Assertion failed: %ls\tLine: %d\tFile: %ls\n",
 //       Info->assertStatement,
 //       Info->assertLineInFile,
 //       Info->assertLocationInFile
 //   );
 //   mbxInfo.Title = "Assertion failed";

    //systemLayer->AddMessageBox(&mbxInfo);
}

void Logger::SendToOutputBuffer(const String* Buffer)
{
    // purposely leaved on empty
}

static constexpr const char* FormatSeverity(LogSeverity Severity) noexcept
{
    switch (Severity)
    {
    case Log:
        return "Log";
    case Warn:
        return "Warning";
    case Error:
        return "Error";
    case Fatal:
        return "Fatal";
    case Verbose:
        return "Verbose";
    }

    return "???";
}