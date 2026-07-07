/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"
#include <Memory/MemoryHandler.h>
#include "Commandline.h"
#include <Special/ImportHelpers.h>
#include <HAL/HAL.h>

#ifdef MR_DEBUG
#include <crtdbg.h>
#endif // MR_DEBUG

#include <Module/ModuleManager.h>

static inline Application* appFramework = nullptr;

Application* Application::Get()
{
    return appFramework;
}

Application::Application()
{
    appFramework = this;

    SetCurrentState(Application::State::PreStartup);
    GetMemoryManager()->Initialize();

    appName = "Meteor Suite(R) application";
    appNameNoSpaces = "MeteorSuite(R)app";
    appCodeName = "MRAPP";
}

void Application::Init()
{
#ifdef MR_DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif // MR_DEBUG
    Logger::Get()->Init();
    HAL::InitEssential();

    ModuleManager::Get().LoadModule("vulkan");
}

void Application::Run()
{
    // physics, audio, rendering?? etc.
}

void Application::Shutdown()
{
    if (currentState == State::Running)
    {
        MR_LOG(LogApplication, Log, "Restarting application!");

        Logger::Get()->Shutdown();

        appFramework->Init();
    }
    else
    {
        MR_LOG(LogApplication, Log, "Shutting down application!");


#ifdef MR_PLATFORM_WINDOWS

#endif // MR_PLATFORM_WINDOWS

        Logger::Get()->Shutdown();

        //GetMemoryManager()->Shutdown();
    }

#ifdef MR_DEBUG
    _CrtDumpMemoryLeaks();
#endif // MR_DEBUG 
}

void Application::RequestExit(u32 code)
{
    SetCurrentState(Application::State::Shutdown);

}

void Application::Tick()
{
    while (GetCurrentState() == State::Running)
    {
        if (HAL::PeekOSMessageQueue())
        {
            appFramework->Run();
        }
    }
}

extern "C" LIBRARY_OUT int LaunchApplication(Application* instance, int argc, char** argv)
{	
    instance->GetCommandline()->Init(argc, argv);

    instance->SetCurrentState(Application::State::Startup);
    instance->Init();

    if (instance->GetCurrentState() == Application::State::Dead)
        return -1;

    instance->SetCurrentState(Application::State::Running);
    instance->Tick();

    return 0; 
}