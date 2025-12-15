/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Application.h"
#include <WindowManager/WindowManager.h>
#include <Platform/FileManager.h>

#include <MemoryManager.h>
#include <Layers/LayerManager.h>
//#include <GraphicsEngine/SceneGraph.h>
#include <mutex>
#include "Commandlet.h"
#ifdef MR_DEBUG
#include <crtdbg.h>
#endif // MR_DEBUG
#include <Platform/PerformanceTimer.h>
#include <Module/ModuleManager.h>
//#include <Renderer/Renderer.h>

Application::Application()
{
    appFramework = this;

    MemoryManager::Get().Initialize(0.15f);
}


void Application::RequestExit(int32_t Code)
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

    MR_LOG(LogApplication, Log, "Initializing application.");


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

        MemoryManager::Get().Shutdown();
    }
}
