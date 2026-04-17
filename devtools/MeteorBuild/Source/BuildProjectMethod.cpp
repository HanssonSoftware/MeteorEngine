/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "BuildProjectMethod.h"
#include <Commandlet.h>
#include <Log.h>
#include <Utils.h>
#include <CommandsRegistry.h>
#include <Platform.h>
#include <VisualStudioProject.h>
#include <Application/Application.h>
#include <Helper.h>
#include <Memory/MemoryHandler.h>
#include <Memory/MemoryBlockArena.h>
#include <Parser.h>
#include <Module.h>
#include <Project.h>

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
#include <Winapi.h>

#include <shlwapi.h>
#include <Objbase.h>
#include <strsafe.h>
#include <PathCch.h>
#include <ShlObj.h>


#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Pathcch.lib")

#pragma warning(disable : 6031)

LOG_ADDCATEGORY(ProjectGeneratorVS);

#define INSERT_SPECIFIER(x, statement) if (!strcmp(verb, x)) { statement return; }

ADD_NEW_BUILD_COMMAND(1, "-build", "builds", CommandBuild);

static void CommandBuild()
{

}

BuildProjectMethod::BuildProjectMethod() : BaseBuildMethod()
{

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

    if (!AcquireRequiredParameters())
        return;

    Array<wchar_t*> files, foundScripts;
    
    wchar_t convertedSourceDir[128] = {};
    Platform::ConvertToWide(convertedSourceDir, 128 - 1, sourceDirectory);

    Utils::ListDirectory(convertedSourceDir, files);
    for (auto& file : files)
    {
        wchar_t* fileWithExtension = file;
        if (SUCCEEDED(PathCchFindExtension(file, wcslen(file) + 1, &fileWithExtension)) && !wcscmp(fileWithExtension, L".mrbuild"))
        {
            foundScripts.Add(file);
            HANDLE script = CreateFileW(file, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
            if (script != INVALID_HANDLE_VALUE)
            {
                LARGE_INTEGER ld;
                GetFileSizeEx(script, &ld);

                DWORD readActually = 0;
                char* buffer = (char*)GetMemoryManager()->Allocate(ld.QuadPart + 1);
                if (ReadFile(script, buffer, (DWORD)ld.QuadPart, &readActually, nullptr))
                {
                    EngineModule* actualModule = ParseModule(buffer);
                    if (actualModule != nullptr)
                    {
                        GenerateGUID(actualModule->identification);
                        //actualModule->modulePath = ConvertPath(file);

                        modules.Add(actualModule);
                    }
                }
                else
                {
                    MR_LOG(LogTemp, Error, "Failed to read %ls script!", file);
                }

                GetMemoryManager()->Deallocate(buffer, ld.QuadPart);
                CloseHandle(script);
            }
            else
            {
                MR_LOG(LogTemp, Error, "Unable to open %ls script!", file);
            }
        }
    }

    if (foundScripts.GetSize() > 0)
    {
        if (Commandlet::Get().Check("-vs2026"))
        {
#ifdef MR_PLATFORM_WINDOWS
            MemoryBlockArena<wchar_t> intermediateProcess = MemoryBlockArena<wchar_t>(16 * 1024 * sizeof(wchar_t));

            wchar_t exeLocation[512] = {};
            const DWORD exeCount = GetModuleFileNameW(nullptr, exeLocation, 511);

            HRCHECK(PathCchRemoveFileSpec(exeLocation, exeCount));

            wchar_t* wideIntermediateString = (wchar_t*)intermediateProcess.Allocate(intermediateDirectory.Length() * sizeof(wchar_t));
            MultiByteToWideChar(CP_UTF8, 0, intermediateDirectory.Chr(), intermediateDirectory.Length(), wideIntermediateString, intermediateDirectory.Length());

            wchar_t appCodeNameWide[256] = {};
            const u32 appCodeNameLength = (u32)strlen(GetApplication()->GetApplicationCodeName());
            MultiByteToWideChar(CP_UTF8, 0, GetApplication()->GetApplicationCodeName(), appCodeNameLength, appCodeNameWide, appCodeNameLength);

            wchar_t* combinedIntermediatePath = (wchar_t*)intermediateProcess.Allocate(exeCount + intermediateDirectory.Length() + 0);
            const u32 combinedIntermediatePathSize = exeCount + intermediateDirectory.Length();

            HRCHECK(PathCchCombine(combinedIntermediatePath, combinedIntermediatePathSize, exeLocation, wideIntermediateString));
            HRCHECK(PathCchCombine(combinedIntermediatePath, combinedIntermediatePathSize + appCodeNameLength, combinedIntermediatePath, appCodeNameWide));

            for (EngineModule*& module : modules)
            {
                wchar_t moduleNameWide[256] = {};
                const u32 convertedModuleNameSize = MultiByteToWideChar(CP_UTF8, 0, module->moduleName, module->moduleName.Length(), moduleNameWide, module->moduleName.Length());

                wchar_t* fullyCompletedPath = (wchar_t*)intermediateProcess.Allocate((combinedIntermediatePathSize + appCodeNameLength + convertedModuleNameSize) * sizeof(wchar_t));

                HRCHECK(PathCchCombine(fullyCompletedPath, combinedIntermediatePathSize + appCodeNameLength + convertedModuleNameSize, combinedIntermediatePath, moduleNameWide));

                SHCreateDirectoryExW(nullptr, fullyCompletedPath, nullptr);

                wchar_t* fullyQualifiedPathToVcxproj = (wchar_t*)intermediateProcess.Allocate((combinedIntermediatePathSize + appCodeNameLength + (convertedModuleNameSize * 2)));
                swprintf(fullyQualifiedPathToVcxproj, (combinedIntermediatePathSize + appCodeNameLength + (convertedModuleNameSize * 2)), L"%ls\\%ls.vcxproj", fullyCompletedPath, moduleNameWide);

                HANDLE vcxprojFile = CreateFileW(fullyQualifiedPathToVcxproj, GENERIC_WRITE,
#ifdef MR_DEBUG
                    FILE_SHARE_READ | FILE_SHARE_WRITE
#else
                    0
#endif // MR_DEBUG 
                    ,nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

                if (vcxprojFile != INVALID_HANDLE_VALUE)
                {
                    VisualStudioTemplate::GenerateHeaderAndConfigurations(vcxprojFile);
                    VisualStudioTemplate::GenerateDynamicDetailsForIdentification(vcxprojFile, &module->moduleName, module->identification);
                    VisualStudioTemplate::GeneratePropertySheetsBoilerplateCode(vcxprojFile);
                    VisualStudioTemplate::GenerateConfigurationRelatedCallsToCompiler(vcxprojFile);
                    VisualStudioTemplate::GenerateClosingBoilerplateCode(vcxprojFile);

                    CloseHandle(vcxprojFile);
                }
                else
                {
                    MR_LOG(LogProjectGeneratorVS, Error, "Failed to create project file for: %ls", moduleNameWide);
                }

                HRCHECK(PathCchRemoveFileSpec(fullyCompletedPath, combinedIntermediatePathSize + appCodeNameLength + convertedModuleNameSize));
            }

#endif // MR_PLATFORM_WINDOWS
        }


        if (BuildSystemLogger* bls = (BuildSystemLogger*)Logger::Get())
        {
            LARGE_INTEGER lgEnd;
            QueryPerformanceCounter(&lgEnd);

            LARGE_INTEGER frq;
            QueryPerformanceFrequency(&frq);

            wchar_t result[64] = {};
            if (swprintf(result, 64, L"Build method is ran successfully in %.2f seconds!", (lgEnd.QuadPart - lg.QuadPart) / (double)frq.QuadPart))
            {
                DWORD actual = 0;
                if (!WriteConsoleW(bls->GetOutputHandle(), result, 64, &actual, nullptr))
                {

                }
            }
        }
    }
    else
    {
        if (BuildSystemLogger* bls = (BuildSystemLogger*)Logger::Get())
        {
            constexpr const wchar_t nofilesBuffer[] = L"No files generated! Check scripts location or appended switches!";

            DWORD actual = 0;
            WriteConsoleW(bls->GetOutputHandle(), nofilesBuffer, (DWORD)wcslen(nofilesBuffer), &actual, nullptr);
        }
    }
}

void BuildProjectMethod::CleanUp()
{
    for (auto& module : modules)
        delete module;

}

EngineModule* BuildProjectMethod::ParseModule(char* buffer)
{
    if (buffer != nullptr)
    {
        int line = 1, current = 1;

        char* begin = buffer;
        char* end = begin;

        EngineModule* newModule = (EngineModule*)GetMemoryManager()->Allocate(sizeof(EngineModule));
        if (Parsing::SkipWord(end, line, current))
        {
            newModule->moduleName = Parsing::GetQuotedWord(end, true);
            if (!Parsing::SkipType(end, Parsing::Colon))
            {
                MR_LOG(LogTemp, Fatal, "Module is not associated with any project! %s", *newModule->moduleName);

                GetMemoryManager()->Deallocate(newModule, sizeof(EngineModule));
                return nullptr;
            }

            newModule->parent = Parsing::GetQuotedWord(end, true);
            if (!Parsing::SkipType(end, Parsing::OpenBrace))
            {
                MR_LOG(LogTemp, Fatal, "Missing open statement!", );

                GetMemoryManager()->Deallocate(newModule, sizeof(EngineModule));
                return nullptr;
            }

            while (Parsing::GetType(end) != Parsing::ClosedBrace)
            { 
                char* actualLocation = end;

                const String word = Parsing::GetWord(end);
                if (word && Parsing::SkipType(end, Parsing::Colon) && Parsing::SkipType(end, Parsing::OpenBrace))
                {
                    while (Parsing::GetType(end) != Parsing::ClosedBrace)
                    {
                        const String entry = Parsing::GetQuotedWord(end, true);
                        SetSpecifierForModule(newModule, word, entry, entry.Length());

                        if (!Parsing::SkipType(end, Parsing::Comma))
                            break;
                    }

                    Parsing::SkipType(end, Parsing::ClosedBrace);
                }

                if (actualLocation == end)
                {
                    MR_LOG(LogTemp, Fatal, "Parser head is not moving!");
                    return nullptr;
                }
            }

            Parsing::SkipType(end, Parsing::ClosedBrace);
        }

        while (isspace(*end))
            end++;

        if (*end != '\0')
        {
            MR_LOG(LogTemp, Fatal, "Failed to parse buffer!");

            GetMemoryManager()->Deallocate(newModule, sizeof(EngineModule));
            return nullptr;
        }

        return newModule;
    }

    return nullptr;
}

inline void BuildProjectMethod::GenerateGUID(char* output)
{
    GUID id;
    CoCreateGuid(&id);

    snprintf(output, 39, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", id.Data1, id.Data2, id.Data3, id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3], id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);
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

inline BuildProjectMethod::ScriptType BuildProjectMethod::DetectScriptType(const char* buffer, u32 length) const noexcept
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

inline void BuildProjectMethod::SetSpecifierForModule(EngineModule* module, const char* verb, const char* verbEntry, u32 length) noexcept
{
    MR_ASSERT(module != nullptr, "Module is invalid!");

    if (module == nullptr)
    {
        MR_LOG(LogTemp, Log, "Unknown specifier! %s", verb);
        return;
    }

    //INSERT_SPECIFIER(L"IncludePath", module->includePaths.Add(verbEntry); );
}

inline void BuildProjectMethod::SetSpecifierForProject(::Project* project, const char* verb, const char* verbEntry, u32 length) noexcept
{
    MR_ASSERT(project != nullptr, "Project is invalid!");

    if (project == nullptr)
    {
        MR_LOG(LogTemp, Log, "Unknown specifier! %s", verb);
        return;
    }

    //INSERT_SPECIFIER("IncludePath");
}

void BuildProjectMethod::FillVcxprojFile(void* fileHandle, EngineModule* module)
{

    char firstPart[] =
        "\t<ItemGroup Label = \"ProjectConfigurations\">\n"
        "\t\t<ProjectConfiguration Include=\"Debug|x64\">\n"
        "\t\t\t<Configuration>Debug</Configuration>\n"
        "\t\t\t<Platform>x64</Platform>\n"
        "\t\t</ProjectConfiguration>\n"
        "\t\t<ProjectConfiguration Include=\"Test|x64\">\n"
        "\t\t\t<Configuration>Test</Configuration>\n"
        "\t\t\t<Platform>x64</Platform>\n"
        "\t\t</ProjectConfiguration>\n"
        "\t\t<ProjectConfiguration Include=\"Shipping|x64\">\n"
        "\t\t\t<Configuration>Shipping</Configuration>\n"
        "\t\t\t<Platform>x64</Platform>\n"
        "\t\t</ProjectConfiguration>\n"
        "\t</ItemGroup>\n"
        "\t<PropertyGroup Label=\"Globals\">\n"
        "\t\t<VCProjectVersion>18.0</VCProjectVersion>\n"
        "\t\t<Keyword>Win32Proj</Keyword>\n"
        "\t\t<ProjectGuid>%ls</ProjectGuid>\n" // 1 project GUID
        "\t\t<RootNamespace>%ls</RootNamespace>\n" // 2 namespace
        "\t\t<WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>\n"
        "\t\t<ProjectName>%ls</ProjectName>\n" // 3 project Name
        "\t</PropertyGroup>\n";
        //
        //	"\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\n"
        //	"\t<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\" Label=\"Configuration\">\n"
        //	"\t\t<ConfigurationType>DynamicLibrary</ConfigurationType>\n"
        //	"\t\t<PlatformToolset>v145</PlatformToolset>\n"
        //	"\t\t<wchar_tacterSet>Unicode</wchar_tacterSet>\n"
        //	"\t\t<UseDebugLibraries>true</UseDebugLibraries>\n"
        //	"\t</PropertyGroup>\n"
        //	"\t<PropertyGroup Condition = \"'$(Configuration)|$(Platform)'=='Shipping|x64'\" Label = \"Configuration\">\n"
        //	"\t\t<ConfigurationType>DynamicLibrary</ConfigurationType>\n"
        //	"\t\t<PlatformToolset>v145</PlatformToolset>\n"
        //	"\t\t<wchar_tacterSet>Unicode</wchar_tacterSet>\n"
        //	"\t\t<WholeProgramOptimization>true</WholeProgramOptimization>\n"
        //	"\t</PropertyGroup>\n"
        //
        //	"\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\"/>\n"
        //	"\t<ImportGroup Label=\"ExtensionSettings\">\n"
        //	"\t</ImportGroup>\n"
        //	"\t<ImportGroup Label=\"Shared\">\n"
        //	"\t</ImportGroup>\n"
        //	"\t<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\n"
        //    "\t\t<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"/>\n"
        //	"\t</ImportGroup>\n"
        //	"\t<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Shipping|x64'\">\n"
        //    "\t\t<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"/>\n"
        //	"\t</ImportGroup>\n"
        //	"\t<PropertyGroup Label=\"UserMacros\" />\n"
        //	
        //	"\t<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\n"
        //	"\t\t<OutDir>$(SolutionDir)Product\\</OutDir>\n"
        //	"\t\t<IntDir>$(SolutionDir)Intermediate\\$(ProjectName)\\</IntDir>\n"
        //	"\t\t<TargetName>MeteorEngine-%ls</TargetName>\n" // 4 module name
        //	"\t</PropertyGroup>\n"
        //	"\t<PropertyGroup Condition = \"'$(Configuration)|$(Platform)'=='Shipping|x64'\">\n"
        //	"\t\t<OutDir>$(SolutionDir)Product\\</OutDir>\n"
        //	"\t\t<IntDir>$(SolutionDir)Intermediate\\$(ProjectName)\\</IntDir>\n"
        //	"\t\t<TargetName>MeteorEngine-%ls</TargetName>\n" // 4 module name
        //	"\t</PropertyGroup>\n"
        //
        //	"\t<ItemDefinitionGroup Condition=\"\'$(Configuration)|$(Platform)\'==\'Debug|x64\'\">\n"
        //	"\t\t<ClCompile>\n"
        //	"\t\t\t<PreprocessorDefinitions>%ls_EXPORTS;MR_DEBUG;%ls</PreprocessorDefinitions>\n"
        //	"\t\t\t<RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>\n"
        //	"\t\t\t<LanguageStandard>stdcpp17</LanguageStandard>\n"
        //	"\t\t\t<Optimization>Disabled</Optimization>\n"
        //	"\t\t\t<BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>\n"
        //	"\t\t\t<AdditionalIncludeDirectories>$(ProjectDir);$(SolutionDir)Intermediate\\;$(SolutionDir)Intermediate\\%ls;%ls</AdditionalIncludeDirectories>\n" // 5 include paths
        //	"\t\t</ClCompile>\n"
        //	"\t</ItemDefinitionGroup>\n"
        //
        //	"\t<ItemDefinitionGroup Condition=\"\'$(Configuration)|$(Platform)\'==\'Shipping|x64\'\">\n"
        //	"\t\t<ClCompile>\n"
        //	"\t\t\t<PreprocessorDefinitions>%ls_EXPORTS;%ls</PreprocessorDefinitions>\n"
        //	"\t\t\t<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>\n"
        //	"\t\t\t<LanguageStandard>stdcpp17</LanguageStandard>\n"
        //	"\t\t\t<Optimization>MaxSpeed</Optimization>\n"
        //	"\t\t\t<AdditionalIncludeDirectories>$(ProjectDir);$(SolutionDir)Intermediate\\;$(SolutionDir)Intermediate\\%ls;%ls</AdditionalIncludeDirectories>\n" // 5 include paths
        //	"\t\t</ClCompile>\n"
        //	"\t</ItemDefinitionGroup>\n"
        //
        //	"\t<ItemGroup>\n"
        //	"\t\t<ClInclude Include=\"%ls.bootstrap.cpp\" />\n"
        //	"%ls"
        //	"\t</ItemGroup>\n"
        //	"\t<ItemGroup>\n"
        //	"\t\t<ClInclude Include=\"%ls.proxy.h\" />\n"
        //	"%ls"
        //	"\t</ItemGroup>\n"
        //	"\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n"
        //	"\t<ImportGroup Label=\"ExtensionTargets\" >\n"
        //	"\t</ImportGroup>\n"
        //	"</Project>",
};
