/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include <cstdlib>

#ifdef MR_DEBUG
#include <crtdbg.h>
#endif // MR_DEBUG


struct Application;

extern "C" __declspec(dllimport) int LaunchApplication(Application* instance, int argc, char argv[]);
          
#ifdef MR_PLATFORM_WINDOWS
#define	WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#include <Windows.h>
#include <PathCch.h>
#include <winuser.h>
#pragma comment (lib, "Pathcch.lib")                                                                     
#pragma comment (lib, "User32.lib")                                                                          
                                                                                                                            
#pragma warning (disable : 28251)                                                                                    
#endif // MR_PLATFORM_WINDOWS                                                                                          

#ifdef MR_PLATFORM_WINDOWS
#pragma warning(disable : 6387)

#define IMPLEMENT_WINDOWS_STARTUP(libName, applicationClass)                                                                             \
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)                                       \
    {                                                                                                                                    \
        wchar_t path[512] = {};                                                                                                          \
        DWORD count = GetModuleFileNameW(hInstance, path, 512);                                                                          \
        if (FAILED(PathCchRemoveFileSpec(path, count)))                                                                                  \
        {                                                                                                                                \
            MessageBoxW(nullptr, L"Unable to canonicalize engine path!", L"Engine Error!", MB_OK);                                       \
            return -1;                                                                                                                   \
        }                                                                                                                                \
                                                                                                                                         \
        AddDllDirectory(path);                                                                                                           \
        HMODULE entryPoint = LoadLibraryExW(L##libName, nullptr, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS);   \
        if (entryPoint != nullptr)                                                                                                       \
        {                                                                                                                                \
            typedef int (*ProxyFunction)(Application*, int, char*);                                                                      \
            ProxyFunction externalLinkageFunction = (ProxyFunction)GetProcAddress(entryPoint, "LaunchApplication");                      \
            if (externalLinkageFunction)                                                                                                 \
            {                                                                                                                            \
                applicationClass* application = new applicationClass;                                                                    \
                int Result = externalLinkageFunction(application, -1, lpCmdLine);                                                        \
                                                                                                                                         \
                if (!FreeLibrary(entryPoint))                                                                                            \
                    return -1;                                                                                                           \
                                                                                                                                         \
                delete application;                                                                                                      \
                return Result;                                                                                                           \
            }                                                                                                                            \
        }                                                                                                                                \
                                                                                                                                         \
        MessageBoxW(nullptr, L"Unable to reach engine code!", L"Engine Error!", MB_OK);                                                  \
        return -1;                                                                                                                       \
    };                                                                                                                                     
#else
#define IMPLEMENT_WINDOWS_STARTUP(libName, applicationClass)
#endif // MR_PLATFORM_WINDOWS

//#ifdef MR_DEBUG                                                                                                                               \
//            _CrtDumpMemoryLeaks();                                                                                                                    \
//        #endif  
//int main(int ArgumentCount, char* Arguments[])                                                                       \
//{                                                                                                                    \
////int Result = LaunchApplication(ArgumentCount, Arguments);                                                      \
                                                                                           \
//    return 0;                                                                                                        \
//}                             
