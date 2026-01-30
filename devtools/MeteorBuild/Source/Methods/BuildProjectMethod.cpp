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
#include <Objbase.h>
#include <strsafe.h>
#include <PathCch.h>
#include <Core/Utils.h>

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

#pragma warning(disable : 6031)

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
            HANDLE script = CreateFileW(file, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
            if (script != INVALID_HANDLE_VALUE)
            {
                LARGE_INTEGER ld;
                GetFileSizeEx(script, &ld);

                DWORD readActually = 0;
                char* buffer = new char[ld.QuadPart + 1]();
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
            newModule->moduleName = GetQuotedWord(end);
            if (!SkipCharacterType(end, Colon))
            {
                MR_LOG(LogTemp, Fatal, "Module is not associated with any project! %s", *newModule->moduleName);
                
                delete newModule;
                return nullptr;
            }

            newModule->parent = GetQuotedWord(end);
            if (!SkipCharacterType(end, OpenBrace))
            {
                MR_LOG(LogTemp, Fatal, "Missing open statement!", );

                delete newModule;
                return nullptr;
            }

            while (GetCharacterType(end) != ClosedBrace)
            { 
                char* actualLocation = end;

                const String word = GetWord(end);
                if (word && SkipCharacterType(end, Colon) && SkipCharacterType(end, OpenBrace))
                {
                    while (GetCharacterType(end) != ClosedBrace)
                    {
                        const String entry = GetQuotedWord(end);
                        SetSpecifierForModule(newModule, word, entry, entry.Length());

                        if (!SkipCharacterType(end, Comma))
                            break;
                    }

                    SkipCharacterType(end, ClosedBrace);
                }

                if (actualLocation == end)
                {
                    MR_LOG(LogTemp, Fatal, "Parser head is not moving!");
                    return nullptr;
                }
            }

            SkipCharacterType(end, ClosedBrace);
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
    char guidBuffer[40] = { '\0' };

    CoCreateGuid(&id);
    sprintf(guidBuffer, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", id.Data1, id.Data2, id.Data3, id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3], id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);

    return guidBuffer;
}

String BuildProjectMethod::ConvertPath(StringW* wideBuffer)
{
    if (!wideBuffer)
        return "";

    wchar_t* wide = wideBuffer->Data();
    
    if (SUCCEEDED(PathCchRemoveFileSpec(wide, wideBuffer->Length())))
    {
        char* buffer = new char[wideBuffer->Length() + 1];
        if (!WideCharToMultiByte(CP_UTF8, 0, wideBuffer->Chr(), wideBuffer->Length(), buffer, wideBuffer->Length() * sizeof(char), nullptr, nullptr))
        {
            int j = 532;
        }

        String output = buffer;

        delete[] buffer;
        return output;
    }

    return String();
}

inline BuildProjectMethod::ScriptType BuildProjectMethod::DetectScriptType(const char* buffer, uint32_t length) const noexcept
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

inline void BuildProjectMethod::SetSpecifierForModule(::Module* module, const char* verb, const char* verbEntry, uint32_t length) noexcept
{
    MR_ASSERT(module != nullptr, "Module is invalid!");

    if (module == nullptr)
    {
        MR_LOG(LogTemp, Log, "Unknown specifier! %s", verb);
        return;
    }

    INSERT_SPECIFIER("IncludePath", module->includePaths.Add(verbEntry); );
}

inline void BuildProjectMethod::SetSpecifierForProject(::Project* project, const char* verb, const char* verbEntry, uint32_t length) noexcept
{
    MR_ASSERT(project != nullptr, "Project is invalid!");

    if (project == nullptr)
    {
        MR_LOG(LogTemp, Log, "Unknown specifier! %s", verb);
        return;
    }

    //INSERT_SPECIFIER("IncludePath");
}