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

#ifdef MR_PLATFORM_WINDOWS
static bool bIsWindowsApplicationClassRegistered = false;

static LRESULT CALLBACK GlobalWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

#endif // MR_PLATFORM_WINDOWS

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
    
    Logger::Get()->Initialize();

#ifdef MR_PLATFORM_WINDOWS
    if (!bIsWindowsApplicationClassRegistered)
    {
        wchar_t buffer[256] = {};
        MultiByteToWideChar(CP_UTF8, 0, (char*)appCodeName.ptr, appCodeName.size, buffer, appCodeName.size);

        WNDCLASSEXW registerData = {};
        registerData.cbSize = sizeof(WNDCLASSEXW);
        registerData.hInstance = GetModuleHandleW(nullptr);
        registerData.lpfnWndProc = GlobalWndProc;
        registerData.lpszClassName = buffer;

        if (!RegisterClassExW(&registerData))
        {
            MR_LOG(LogApplication, Fatal, "Unable to register engine critical component to Windows! RegisterClassExW=%d", GetLastError());
        }

        bIsWindowsApplicationClassRegistered = true;
    }
#endif // MR_PLATFORM_WINDOWS
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


#ifdef MR_PLATFORM_WINDOWS
        if (bIsWindowsApplicationClassRegistered)
        {
            wchar_t buffer[256] = {};
            MultiByteToWideChar(CP_UTF8, 0, (char*)appCodeName.ptr, appCodeName.size, buffer, appCodeName.size);

            if (!UnregisterClassW(buffer, GetModuleHandleW(nullptr)))
            {
                MR_LOG(LogApplication, Fatal, "Failed to unregister engine critical component from Windows! UnregisterClassW=%d", GetLastError());
            }

            bIsWindowsApplicationClassRegistered = false;
        }
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
    instance->SetCurrentState(Application::State::PreStartup);
    GetMemoryManager()->Initialize();

    instance->GetCommandline()->Init(argc, argv);

    instance->SetCurrentState(Application::State::Startup);
    instance->Init();

    if (instance->GetCurrentState() == Application::State::Dead)
        return -1;

    return 0; 
}