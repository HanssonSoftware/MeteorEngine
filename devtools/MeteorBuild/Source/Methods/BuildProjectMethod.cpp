/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "BuildProjectMethod.h"
#include <Commandlet.h>


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
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE 
#define NOMINMAX 
#define NOMSG
#define NOOPENFILE 
#define NOSCROLL 
// #define NOSERVICE
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

#include <strsafe.h>


BuildProjectMethod::BuildProjectMethod() : BaseBuildMethod()
{
    name = StringW(L"Build Project");
}

bool BuildProjectMethod::AcquireRequiredParameters()
{
    if (Commandlet::Get().Parse("-source", ) || Commandlet::Get().Parse("-src", nullptr) || Commandlet::Get().Parse("-s", nullptr))
    {
        
    }
    else if (Commandlet::Get().Parse("-intermediate", nullptr) || Commandlet::Get().Parse("-intermediate", nullptr))
    {
        //currentMethod = new RebuildProjectMethod;
    }

    return true;
}

void BuildProjectMethod::BeginCreating()
{
    LARGE_INTEGER lg;
    QueryPerformanceCounter(&lg);
    start = lg.QuadPart;

    for (int i = 0; i < 50000; i++)
    {
        int j = i;
    }

    QueryPerformanceCounter(&lg);
    end = lg.QuadPart;

    LARGE_INTEGER frq;
    QueryPerformanceFrequency(&frq);

    wchar_t result[256] = {};
    StringCchPrintfW(result, 256, L"%s method is ran successfully in %.2f seconds!", *name, (end - start) / (double)frq.QuadPart);

    DWORD actual = 0;
    if (!WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), result, 256, &actual, nullptr))
    {

    }
}
