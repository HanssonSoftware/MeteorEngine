/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"
#include <Memory/MemoryHandler.h>
#include "Commandline.h"
#include <Special/ImportHelpers.h>
#include <HAL/Window.h>
#include <HAL/HAL.h>

#ifdef MR_DEBUG
#include <crtdbg.h>
#endif // MR_DEBUG

#include <Module/ModuleManager.h>
#include <Timer.h>

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
}

void Application::Init()
{
    SetCurrentState(Application::State::Startup);

#ifdef MR_DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif // MR_DEBUG
    Logger::Get()->Init();
    MR_LOG(LogApplication, Log, "Initalizing application");

    HAL::InitHAL();

    if (!bHideWindow) mainWindow = Window::Create(windowDefs.windowName.Chr(), 10, 10, windowDefs.x, windowDefs.y);

    // this is hardcoded!

    if (mainWindow) mainWindow->Show();
}

void Application::Run(float dt)
{
    ModuleManager::Get().UpdateModules(dt);
    // physics, audio, rendering ( done :) )?? etc.
}

void Application::Shutdown()
{
    ModuleManager::Get().ShutdownModules();

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

extern "C" LIBRARY_OUT int LaunchApplication(Application* instance, int argc, char** argv)
{	
    instance->GetCommandline()->Init(argc, argv);
    instance->Init();

    if (instance->GetCurrentState() == Application::State::Dead)
        return -1;

    instance->SetCurrentState(Application::State::Running);
    while (instance->GetCurrentState() == Application::State::Running)
    {
        if (!HAL::PeekOSMessageQueue())
            break;

        appFramework->Run(-662);
    }

    instance->Shutdown();

    return 0; 
}