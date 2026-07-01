/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"
#include <Memory/MemoryHandler.h>
#include "Commandline.h"
#include <Special/ImportHelpers.h>

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

    appName = { "Meteor Suite(R) application" };
    appNameNoSpaces = { "MeteorSuite(R)app" };
    appCodeName = { "MRAPP" };
}

void Application::Init()
{
#ifdef MR_DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif // MR_DEBUG
    
    GetMemoryManager()->Initialize();
    Logger::Get()->Initialize();
}

void Application::Run()
{
    appFramework->Run();
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

        Logger::Get()->Shutdown();

        //GetMemoryManager()->Shutdown();
    }

#ifdef MR_DEBUG
    _CrtDumpMemoryLeaks();
#endif // MR_DEBUG 
}

void Application::RequestExit(u32 code)
{

}

extern "C" LIBRARY_OUT int LaunchApplication(Application* instance, int argc, char** argv)
{	
    GetMemoryManager()->Initialize();

    instance->GetCommandline()->Init(argc, argv);
    instance->Init();

    return 0; 
}