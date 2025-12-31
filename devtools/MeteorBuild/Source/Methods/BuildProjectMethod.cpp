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

#include <shlwapi.h>
#include <strsafe.h>
#include <PathCch.h>
#include <Core/Utils.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

BuildProjectMethod::BuildProjectMethod() : BaseBuildMethod()
{
    name = StringW(L"Build Project");
}

bool BuildProjectMethod::AcquireRequiredParameters()
{
    uint32_t hasRequired = 0;

    String source, intermediate, solution;
    if (Commandlet::Get().Parse("-source", &source) || Commandlet::Get().Parse("-src", &source) || Commandlet::Get().Parse("-s", &source))
        hasRequired++;
    
    if (Commandlet::Get().Parse("-intermediate", &intermediate) || Commandlet::Get().Parse("-int", &intermediate))
        hasRequired++;

    if (Commandlet::Get().Parse("-solution", &solution) || Commandlet::Get().Parse("-sln", &solution))
    {
        alternativeSolutionDir = solution;
    }

    sourceDirectory = source;
    intermediateDirectory = intermediate;
    return hasRequired == 2 ? true : false;
}

void BuildProjectMethod::BeginCreating()
{
    LARGE_INTEGER lg;
    QueryPerformanceCounter(&lg);
    start = lg.QuadPart;

    MR_ASSERT(sourceDirectory, "Source directory parameter missing!");

    StringW formattedSourceDirectory = sourceDirectory;

    Array<StringW> collectedSourcesWithScripts;
    Utils::ListDirectory(formattedSourceDirectory.Data(), collectedSourcesWithScripts);
    for (auto& file : collectedSourcesWithScripts)
    {
        wchar_t* ptr = file.Data();
        if (SUCCEEDED(PathCchFindExtension(file.Data(), file.Length() + 1, &ptr)) && !wcscmp(ptr, L".mrbuild"))
        {
            foundScripts.Add(file);
            HANDLE script = CreateFileW(file, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (script != INVALID_HANDLE_VALUE)
            {
                char moduleDefine[10] = { '\0' };

                DWORD readActually = 0;
                if (ReadFile(script, moduleDefine, sizeof(moduleDefine) / sizeof(moduleDefine[0]) - 1, &readActually, nullptr) > 0)
                {
                    char* base = moduleDefine;

                    int count = 0;
                    while (!isspace(*base))
                    {
                        base++; count++;
                    }

                    if (strncmp(moduleDefine, "Module", count) == 0)
                    {
                        int j = 5235;
                    }
                    else if (strncmp(moduleDefine, "Project", count) == 0)
                    {
                        int j = 5235;
                    }
                }
                else
                {
                    //MR_LOG(LogBuildSystemFramework, Error, "Failed to read minimum amounts of bytes from file! %ls", *script);
                }

                CloseHandle(script);
            }
        }
    }
}

void BuildProjectMethod::Finalize()
{
    LARGE_INTEGER lg;
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
