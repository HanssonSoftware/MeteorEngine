/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#ifdef MR_DEBUG
#include <crtdbg.h>
#endif // MR_DEBUG
          
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
#define OEMRESOURCE                                                                                                  
#define NOATOM                                                                                                       
#define NOCLIPBOARD                                                                                                  
#define NOCOLOR                                                                                                      
#define NOCTLMGR                                                                                                     
#define NODRAWTEXT                                                                                                   
#define NOGDI                                                                                                        
#define NOKERNEL                                                                                                                                                                                                         \
#define NONLS                                                                                                                                                                                                             \
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
#pragma comment (lib, "Pathcch.lib")                                                                                 
                                                                                                                            
#pragma warning (disable : 28251)                                                                                    
#endif // MR_PLATFORM_WINDOWS                                                                                          

extern "C" __declspec(dllexport) int LaunchApplication(int ArgumentCount, char** Arguments);

#ifdef MR_PLATFORM_WINDOWS
#define IMPLEMENT_WINDOWS_STARTUP(libName)\
    int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)                                              \
    {                                                                                                                                           \
        wchar_t path[512] = { '\0' };                                                                                                           \
        DWORD count = GetModuleFileNameW(GetModuleHandleW(nullptr), path, 512);                                                                 \
        PathCchRemoveFileSpec(path, count);                                                                                                     \
        \
        AddDllDirectory(path);                                                                                                                  \
        HMODULE entryPoint = LoadLibraryExW(L##libName, nullptr, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS);          \
        if (!entryPoint)                                                                                                                        \
           return -1;                                                                                                                          \
        \
        auto a = GetLastError();\
        using LA = int (*)(int, char**); \
        LA app = (LA)GetProcAddress(entryPoint, "LaunchApplication");                                             \
        if (app)                                                                                                                                \
        {                                                                                                                                       \
            int Result = app(0, &lpCmdLine);                                                                                                    \
            \
            if (!FreeLibrary(entryPoint))                                                                                                       \
            return -1;                                                                                                                      \
            \
            return Result;                                                                                                                      \
        }                                                                                                                                       \
        \
        auto b = GetLastError();                                                                                                                \
        MessageBoxW(nullptr, L"Unable to reach engine code!", L"Engine Error!", MB_OK);                                                         \
        return -1;                                                                                                                              \
    };
#else
#define IMPLEMENT_WINDOWS_STARTUP(libName)
#endif // MR_PLATFORM_WINDOWS


//#ifdef MR_DEBUG                                                                                                                               \
//            _CrtDumpMemoryLeaks();                                                                                                                    \
//        #endif  
//int main(int ArgumentCount, char* Arguments[])                                                                       \
//{                                                                                                                    \
////int Result = LaunchApplication(ArgumentCount, Arguments);                                                      \
//#ifdef MR_DEBUG                                                                                                      \
//    _CrtDumpMemoryLeaks();                                                                                           \
//#endif // MR_DEBUG                                                                                                   \
//    return 0;                                                                                                        \
//}                             
