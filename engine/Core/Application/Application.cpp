/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"
#include <Memory/MemoryHandler.h>
#include "Commandlet.h"

#ifdef MR_DEBUG
#include <crtdbg.h>
#endif // MR_DEBUG

#include <Module/ModuleManager.h>

void Application::RequestExit(int Code)
{
    appFramework->exitCode = Code;
    appFramework->SetAppState(ECurrentApplicationState::SHUTDOWN);
}

void Application::Init()
{
#ifdef MR_DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif // MR_DEBUG
    
    Logger::Get()->Initialize();

    SetAppState(ECurrentApplicationState::RUNNING);
}

void Application::Run()
{
    while (GetAppState() == ECurrentApplicationState::RUNNING)
    {
        appFramework->Run();
    }
}

void Application::Shutdown()
{
    if (GetAppState() == ECurrentApplicationState::RESTARTING)
    {
        MR_LOG(LogApplication, Log, "Restarting application!");

        Logger::Get()->Shutdown();

        appFramework->Init();
    }
    else
    {
        MR_LOG(LogApplication, Log, "Shutting down application!");

        Logger::Get()->Shutdown();

        //GetMemoryManager()->Shutdown();
    }

#ifdef MR_DEBUG
    _CrtDumpMemoryLeaks();
#endif // MR_DEBUG 
}

#ifdef MR_PLATFORM_WINDOWS
extern "C" __declspec(dllexport) int LaunchApplication(Application* instance, int argc, wchar_t argv[])
#else
extern "C" __declspec(dllexport) int LaunchApplication(Application* instance, int argc, char argv[])
#endif // MR_PLATFORM_WINDOWS
{	
#ifdef MR_PLATFORM_WINDOWS
    
#endif // MR_PLATFORM_WINDOWS

    int returnCode = 0xDEADBEEF; 
    if (instance != nullptr)
    {
        Commandlet::Get().Initialize(argc, argv);
        GetMemoryManager()->Initialize();
        instance->Init();
        returnCode = instance->GetRequestExitCode();
        Commandlet::Get().Shutdown();
    }

    return returnCode; 
}