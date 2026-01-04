/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "BuildProjectMethod.h"
#include <Commandlet.h>
#include <Core/Log.h>
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
#include <strsafe.h>
#include <PathCch.h>
#include <Core/Utils.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

#define INSERT_SPECIFIER(x, statement) if (!strncmp(verb, x, length)) { statement return; }

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
        alternativeSolutionDir = solution;

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
                if (ReadFile(script, moduleDefine, (sizeof(moduleDefine) / sizeof(moduleDefine[0])) - 1, &readActually, nullptr) > 0)
                {
                    readActually = 0;

                    char* base = moduleDefine;
                    char* end = base;

                    while (!isspace(*end))
                        end++;

                    LARGE_INTEGER lg;
                    GetFileSizeEx(script, &lg);

                    SetFilePointer(script, 0, 0, FILE_BEGIN);

                    char* buffer = new char[lg.QuadPart + 1]();
                    if (ReadFile(script, buffer, (DWORD)lg.QuadPart, &readActually, nullptr) > 0)
                    {
                        switch (DetectScriptType(moduleDefine, end - base))
                        {
                        case ScriptType::Project:
                            ParseProject(buffer);
                            break;
                        case ScriptType::Module:
                            ParseModule(buffer);
                            break;

                        default:
                            MR_LOG(LogTemp, Error, "Unable to detect script type!");
                            break;
                        }
                    }

                    delete[] buffer;
                }
                else
                {
                    MR_LOG(LogTemp, Error, "Failed to read minimum amounts of bytes from file! %ls", *file);
                }

                CloseHandle(script);
            }
            else
            {
                MR_LOG(LogTemp, Error, "Unable to open %ls script!", *file);
            }
        }
    }
}

void BuildProjectMethod::Finalize()
{
    if (BuildSystemLogger* bls = (BuildSystemLogger*)Logger::Get())
    {
        LARGE_INTEGER lg;
        QueryPerformanceCounter(&lg);

        LARGE_INTEGER frq;
        QueryPerformanceFrequency(&frq);

        wchar_t result[256] = {};
        if (SUCCEEDED((result, 256, L"%s method is ran successfully in %.2f seconds!", *name, (lg.QuadPart - start) / (double)frq.QuadPart)))
        {
            DWORD actual = 0;
            if (!WriteConsoleW(bls->GetOutputHandle(), result, 256, &actual, nullptr))
            {

            }
        }
    }

    CleanUp();
}

void BuildProjectMethod::CleanUp()
{
    for (auto& module : modules)
        delete module;

}

Project* BuildProjectMethod::ParseProject(char* buffer)
{
    if (buffer != nullptr)
    {
        char* begin = buffer;
        char* end = begin;

        //ReadFile(fileHandle, )
        //return ;
    }

    return nullptr;
}

Module* BuildProjectMethod::ParseModule(char* buffer)
{
    if (buffer != nullptr)
    {
        int line = 1, character = 1;

        char* begin = buffer;
        char* end = begin;

        ::Module* newModule = new ::Module;
        if (SkipWord(end, line, character))
        {
            newModule->moduleName = GetWord(end);
            if (!SkipCharacterType(end, Colon))
            {
                MR_LOG(LogTemp, Fatal, "Module is not associated with any project! %s", *newModule->moduleName);
                
                delete newModule;
                return nullptr;
            }

            newModule->dependsOn = GetWord(end);
            while (GetCharacterType(end) != EndOfFile)
            {
                if (SkipCharacterType(end, OpenBrace))
                {
                    if (String verb = GetWord(end))
                    {
                        if (SkipCharacterType(end, Colon) && SkipCharacterType(end, OpenBrace))
                        {
                            while (GetCharacterType(end) != ClosedBrace)
                            {
                                SkipCharacterType(end, Comma);

                                String entry = GetWord(end);
                                SetSpecifier(newModule, verb, entry, entry.Length());
                            }
                        }
                    }
                }
            }

            int j = 23;
        }


        if (*end != '\0')
        {
            MR_LOG(LogTemp, Fatal, "Failed to parse buffer!");

            delete newModule;
            return nullptr;
        }

        return nullptr;
    }

    return nullptr;
}

BuildProjectMethod::ScriptType BuildProjectMethod::DetectScriptType(const char* buffer, uint32_t length) const noexcept
{
    if (!strncmp(buffer, "Project", length))
    {
        return ScriptType::Project;
    }
    else if (!strncmp(buffer, "Module", length))
    {
        return ScriptType::Module;
    }

    return ScriptType::None;
}

void BuildProjectMethod::SetSpecifier(::Module* module, const char* verb, const char* verbEntry, uint32_t length) noexcept
{
    MR_ASSERT(module != nullptr, "Module is invalid!");

    if (module == nullptr)
    {
        MR_LOG(LogTemp, Log, "Unknown specifier! %s", verb);
        return;
    }

    INSERT_SPECIFIER("IncludePath", module->includePaths.Add(verbEntry); );
}