/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#define _CRT_NON_CONFORMING_SWPRINTFS
#include "BuildProjectMethod.h"
#include <Commandlet.h>
#include <Core/Log.h>
#include <Resource/MemoryManager.h>
#include <Module/Parser.h>
#include <Module/Module.h>
#include <Module/Project.h>

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
#include <Objbase.h>
#include <strsafe.h>
#include <PathCch.h>
#include <Core/Utils.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

#pragma warning(disable : 6031)

#define INSERT_SPECIFIER(x, statement) if (!wcscmp(verb, x)) { statement return; }

BuildProjectMethod::BuildProjectMethod() : BaseBuildMethod()
{
    name = "Build Project";
}

bool BuildProjectMethod::AcquireRequiredParameters()
{
    sourceDirectory = 
        Commandlet::Get().Check("-source") ? Commandlet::Get().Parse("-source") : 
        Commandlet::Get().Check("-src") ? Commandlet::Get().Parse("-src") : 
        Commandlet::Get().Parse("-s") ? Commandlet::Get().Parse("-s") : String("");

    intermediateDirectory = 
        Commandlet::Get().Check("-intermediate") ? Commandlet::Get().Parse("-intermediate") :
        Commandlet::Get().Check("-int") ? Commandlet::Get().Parse("-int") :
        Commandlet::Get().Parse("-i") ? Commandlet::Get().Parse("-i") : String("");

    alternativeSolutionDir = 
        Commandlet::Get().Check("-solution") ? Commandlet::Get().Parse("-solution") :
        Commandlet::Get().Parse("-sln") ? Commandlet::Get().Parse("-sln") : String("");

    return sourceDirectory && intermediateDirectory;
}

void BuildProjectMethod::StartMethod()
{
    LARGE_INTEGER lg;
    QueryPerformanceCounter(&lg);
    start = lg.QuadPart;

    Array<wchar_t*> collectedSourcesWithScripts;
    //Utils::ListDirectory(formattedSourceDirectory.Data(), collectedSourcesWithScripts);
#if 0
    if (collectedSourcesWithScripts.GetSize() > 0)
    {
        for (auto& file : collectedSourcesWithScripts)
        {
            wchar_t* ptr = file.Data();
            if (SUCCEEDED(PathCchFindExtension(file.Data(), file.Length() + 1, &ptr)) && !wcscmp(ptr, L".mrbuild"))
            {
                foundScripts.Add(file);
                HANDLE script = CreateFileW(file, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
                if (script != INVALID_HANDLE_VALUE)
                {
                    LARGE_INTEGER ld;
                    GetFileSizeEx(script, &ld);

                    DWORD readActually = 0;
                    wchar_t* buffer = (wchar_t*)GetMemoryManager()->Allocate(ld.QuadPart + 1);
                    if (ReadFile(script, buffer, (DWORD)ld.QuadPart, &readActually, nullptr))
                    {
                        ::Module* actualModule = ParseModule(buffer);
                        if (actualModule != nullptr)
                        {
                            actualModule->identification = GenerateGUID();
                            actualModule->modulePath = ConvertPath(&file);

                            modules.Add(actualModule);
                        }
                    }
                    else
                    {
                        MR_LOG(LogTemp, Error, "Failed to read minimum amounts of bytes from file! %ls", *file);
                    }

                    delete[] buffer;
                    CloseHandle(script);
                }
                else
                {
                    MR_LOG(LogTemp, Error, "Unable to open %ls script!", *file);
                }
            }
        }

        if (BuildSystemLogger* bls = (BuildSystemLogger*)Logger::Get())
        {
            LARGE_INTEGER lgEnd;
            QueryPerformanceCounter(&lgEnd);

            LARGE_INTEGER frq;
            QueryPerformanceFrequency(&frq);

            wchar_t result[256] = {};
            if (SUCCEEDED(swprintf(result, 256, L"%hs method is ran successfully in %.2f seconds!", *name, (lgEnd.QuadPart - lg.QuadPart) / (double)frq.QuadPart)))
            {
                DWORD actual = 0;
                if (!WriteConsoleW(bls->GetOutputHandle(), result, 256, &actual, nullptr))
                {

                }
            }
        }

    }
#endif
}

void BuildProjectMethod::CleanUp()
{
    for (auto& module : modules)
        delete module;

}

Module* BuildProjectMethod::ParseModule(wchar_t* buffer)
{
    if (buffer != nullptr)
    {
        int line = 1, wchar_tacter = 1;

        wchar_t* begin = buffer;
        wchar_t* end = begin;

        ::Module* newModule = new ::Module;
        if (SkipWord(end, line, wchar_tacter))
        {
            newModule->moduleName = GetQuotedWord(end);
            if (!SkipType(end, Colon))
            {
                MR_LOG(LogTemp, Fatal, "Module is not associated with any project! %s", *newModule->moduleName);
                
                delete newModule;
                return nullptr;
            }

            newModule->parent = GetQuotedWord(end);
            if (!SkipType(end, OpenBrace))
            {
                MR_LOG(LogTemp, Fatal, "Missing open statement!", );

                delete newModule;
                return nullptr;
            }

            while (GetType(end) != ClosedBrace)
            { 
                wchar_t* actualLocation = end;

                const String word = GetWord(end);
                if (word && SkipType(end, Colon) && SkipType(end, OpenBrace))
                {
                    while (GetType(end) != ClosedBrace)
                    {
                        const String entry = GetQuotedWord(end);
                        //SetSpecifierForModule(newModule, word, entry, entry.Length());

                        if (!SkipType(end, Comma))
                            break;
                    }

                    SkipType(end, ClosedBrace);
                }

                if (actualLocation == end)
                {
                    MR_LOG(LogTemp, Fatal, "Parser head is not moving!");
                    return nullptr;
                }
            }

            SkipType(end, ClosedBrace);
        }

        while (isspace(*end))
            end++;

        if (*end != '\0')
        {
            MR_LOG(LogTemp, Fatal, "Failed to parse buffer!");

            delete newModule;
            return nullptr;
        }

        return newModule;
    }

    return nullptr;
}

inline String BuildProjectMethod::GenerateGUID() const
{
    GUID id;
    char guidBuffer[40] = { L'\0' };

    CoCreateGuid(&id);
    snprintf(guidBuffer, 40, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", id.Data1, id.Data2, id.Data3, id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3], id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);
    return guidBuffer;
}

String BuildProjectMethod::ConvertPath(String* wideBuffer)
{
    if (!wideBuffer)
        return "";

    //wchar_t* wide = wideBuffer->Data();
    
    //if (SUCCEEDED(PathCchRemoveFileSpec(wide, wideBuffer->Length())))
    {
        //wchar_t* buffer = (wchar_t*)GetMemoryManager()->Allocate(wideBuffer->Length() + 1);
        //if (!Widewchar_tToMultiByte(CP_UTF8, 0, wideBuffer->Chr(), wideBuffer->Length(), buffer, wideBuffer->Length() * sizeof(wchar_t), nullptr, nullptr))
        //{
        //    int j = 532;
        //}

        //String output = buffer;

        //delete[] buffer;
        //return wide;
    }

    return "";
}

inline BuildProjectMethod::ScriptType BuildProjectMethod::DetectScriptType(const wchar_t* buffer, uint32_t length) const noexcept
{
    if (!wcsncmp(buffer, L"Project", length))
    {
        return ScriptType::Project;
    }
    else if (!wcsncmp(buffer, L"Module", length))
    {
        return ScriptType::Module;
    }

    return ScriptType::None;
}

inline void BuildProjectMethod::SetSpecifierForModule(::Module* module, const wchar_t* verb, const wchar_t* verbEntry, uint32_t length) noexcept
{
    MR_ASSERT(module != nullptr, "Module is invalid!");

    if (module == nullptr)
    {
        MR_LOG(LogTemp, Log, "Unknown specifier! %s", verb);
        return;
    }

    //INSERT_SPECIFIER(L"IncludePath", module->includePaths.Add(verbEntry); );
}

inline void BuildProjectMethod::SetSpecifierForProject(::Project* project, const wchar_t* verb, const wchar_t* verbEntry, uint32_t length) noexcept
{
    MR_ASSERT(project != nullptr, "Project is invalid!");

    if (project == nullptr)
    {
        MR_LOG(LogTemp, Log, "Unknown specifier! %s", verb);
        return;
    }

    //INSERT_SPECIFIER("IncludePath");
}