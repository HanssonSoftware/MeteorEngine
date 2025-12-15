/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#define _CRT_SECURE_NO_WARNINGS
//#include <stdlib.h>

#ifdef MR_DEBUG
#include <crtdbg.h>
#endif // MR_DEBUG

#include <Platform/PlatformLayout.h>

#ifdef MR_PLATFORM_WINDOWS
#include <Windows.h>
#include <PathCch.h>

#pragma comment (lib, "Pathcch.lib")
#endif // MR_PLATFORM_WINDOWS

#pragma warning (disable : 28251)

typedef int (*LaunchApplication)(int ArgumentCount, char* Arguments[]);
//extern int LaunchApplication(int ArgumentCount, char* Arguments[]);

int main(int ArgumentCount, char* Arguments[])
{
    //int Result = LaunchApplication(ArgumentCount, Arguments);
#ifdef MR_DEBUG
    _CrtDumpMemoryLeaks();
#endif // MR_DEBUG
    return 0;
}

#ifdef MR_PLATFORM_WINDOWS
// DO NOT MAKE STATIC!!
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    wchar_t path[512] = { '\0' };
    DWORD count = GetModuleFileNameW(GetModuleHandleW(nullptr), path, 512);
    PathCchRemoveFileSpec(path, count);

    AddDllDirectory(/*wcscat(path, L"\\Product\0")*/ path);

    //LaunchApplication(0, &lpCmdLine); // fixme
#if 1
    HMODULE entryPoint = LoadLibraryExW(L"MeteorEngine-Core.dll", nullptr, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS);
    if (!entryPoint) 
        return -1;

    auto a = GetLastError();
    LaunchApplication app = (LaunchApplication)GetProcAddress(entryPoint, "LaunchApplication");
    if (app)
    {
        int Result = app(/* Unused --> */ 0, &lpCmdLine);

        if (!FreeLibrary(entryPoint)) 
            return -1;

#ifdef MR_DEBUG
        _CrtDumpMemoryLeaks();
#endif // MR_DEBUG
        return Result;
    }

    auto b = GetLastError();
    MessageBoxW(nullptr, L"Unable to reach engine code!", L"Engine Error!", MB_OK);
#endif // 0
    return -1;
}

#endif // MR_PLATFORM_WINDOWS