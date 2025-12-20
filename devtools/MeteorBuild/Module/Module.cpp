/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Module.h"
#include <Platform/FileManager.h>
#include <Platform/File.h>
#include <Core/Utils.h>
#include <Core/Application.h>
#include "Project.h"

#include <rpc.h>
#include <Objbase.h>

#pragma warning(disable : 6031) // Return value ignored

LOG_ADDCATEGORY(Parser);

bool Module::ConstructProjectFile(String* output)
{
	String compileList, includeList, other;

	for (auto& cl : files)
	{
		if (FileManager::IsEndingWith(cl, "h"))
		{
			String actual = String::Format("\t\t<ClInclude Include=\"%ls\" />\n", *cl);

			includeList = String::Format("%ls%ls", *includeList, *actual);
		}
		else if (FileManager::IsEndingWith(cl, "cpp"))
		{
			String actual = String::Format("\t\t<ClCompile Include=\"%ls\" />\n", *cl);
			compileList = String::Format("%ls%ls", *compileList, *actual);
		}
		else
		{
			//String actual = String::Format("\t\t<ClCompile Include=\"%ls\" />\n", *cl);
			//compileList = String::Format("%ls%ls", *compileList, *actual);
		}
	}

	String defines;
	String includePath;
	if (BuildSystemApplication* app = GetApplication<BuildSystemApplication>())
	{
		const Project* projectScript = &app->GetBuildSystem().GetProjectScript();
		for (auto& df : projectScript->globalDefines)
		{
			defines = String::Format("%ls;%ls", *defines, *df);
		}

		defines += ";%(PreprocessorDefinitions)";

		const Array<Module>* mdls = app->GetBuildSystem().GetModules();
		for (auto& mdl : *mdls)
		{
			for (auto& rq : requires)
			{
				if (mdl.moduleName == rq)
				{
					for (auto& ic : mdl.includePaths)
					{
						includePath = String::Format("%ls;%ls", *includePath, *ic);
					}
				}
			}			
		}

		for (auto& ic : includePaths)
		{
			includePath = String::Format("%ls;%ls", *includePath, *ic);
		}
	}

	*output = String::Format(
	"<!-- This file is generated with MeteorBuild(R) -->\n\n"
	"<Project DefaultTargets=\"Build\" xmlns=\'http://schemas.microsoft.com/developer/msbuild/2003\'>\n"
	"\t<ItemGroup Label = \"ProjectConfigurations\">\n"
	"\t\t<ProjectConfiguration Include=\"Debug|x64\">\n"
	"\t\t\t<Configuration>Debug</Configuration>\n"
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

	"\t</PropertyGroup>\n"
	"\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\n"
	"\t<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\" Label=\"Configuration\">\n"
	"\t\t<ConfigurationType>DynamicLibrary</ConfigurationType>\n"
	"\t\t<PlatformToolset>v145</PlatformToolset>\n"
	"\t\t<CharacterSet>Unicode</CharacterSet>\n"
	"\t\t<UseDebugLibraries>true</UseDebugLibraries>\n"
	"\t</PropertyGroup>\n"
	"\t<PropertyGroup Condition = \"'$(Configuration)|$(Platform)'=='Shipping|x64'\" Label = \"Configuration\">\n"
	"\t\t<ConfigurationType>DynamicLibrary</ConfigurationType>\n"
	"\t\t<PlatformToolset>v145</PlatformToolset>\n"
	"\t\t<CharacterSet>Unicode</CharacterSet>\n"
	"\t\t<WholeProgramOptimization>true</WholeProgramOptimization>\n"
	"\t</PropertyGroup>\n"

	"\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\"/>\n"
	"\t<ImportGroup Label=\"ExtensionSettings\">\n"
	"\t</ImportGroup>\n"
	"\t<ImportGroup Label=\"Shared\">\n"
	"\t</ImportGroup>\n"
	"\t<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\n"
    "\t\t<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"/>\n"
	"\t</ImportGroup>\n"
	"\t<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Shipping|x64'\">\n"
    "\t\t<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\"/>\n"
	"\t</ImportGroup>\n"
	"\t<PropertyGroup Label=\"UserMacros\" />\n"
	
	"\t<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\n"
	"\t\t<OutDir>$(SolutionDir)Product\\</OutDir>\n"
	"\t\t<IntDir>$(SolutionDir)Intermediate\\$(ProjectName)\\</IntDir>\n"
	"\t\t<TargetName>MeteorEngine-%ls</TargetName>\n" // 4 module name
	"\t</PropertyGroup>\n"
	"\t<PropertyGroup Condition = \"'$(Configuration)|$(Platform)'=='Shipping|x64'\">\n"
	"\t\t<OutDir>$(SolutionDir)Product\\</OutDir>\n"
	"\t\t<IntDir>$(SolutionDir)Intermediate\\$(ProjectName)\\</IntDir>\n"
	"\t\t<TargetName>MeteorEngine-%ls</TargetName>\n" // 4 module name
	"\t</PropertyGroup>\n"

	"\t<ItemDefinitionGroup Condition=\"\'$(Configuration)|$(Platform)\'==\'Debug|x64\'\">\n"
	"\t\t<ClCompile>\n"
	"\t\t\t<PreprocessorDefinitions>%ls_EXPORTS;MR_DEBUG;%ls</PreprocessorDefinitions>\n"
	"\t\t\t<RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>\n"
	"\t\t\t<LanguageStandard>stdcpp17</LanguageStandard>\n"
	"\t\t\t<Optimization>Disabled</Optimization>\n"
	"\t\t\t<BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>\n"
	"\t\t\t<AdditionalIncludeDirectories>$(ProjectDir);$(SolutionDir)Intermediate\\;$(SolutionDir)Intermediate\\%ls;%ls</AdditionalIncludeDirectories>\n" // 5 include paths
	"\t\t</ClCompile>\n"
	"\t</ItemDefinitionGroup>\n"

	"\t<ItemDefinitionGroup Condition=\"\'$(Configuration)|$(Platform)\'==\'Shipping|x64\'\">\n"
	"\t\t<ClCompile>\n"
	"\t\t\t<PreprocessorDefinitions>%ls_EXPORTS;%ls</PreprocessorDefinitions>\n"
	"\t\t\t<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>\n"
	"\t\t\t<LanguageStandard>stdcpp17</LanguageStandard>\n"
	"\t\t\t<Optimization>MaxSpeed</Optimization>\n"
	"\t\t\t<AdditionalIncludeDirectories>$(ProjectDir);$(SolutionDir)Intermediate\\;$(SolutionDir)Intermediate\\%ls;%ls</AdditionalIncludeDirectories>\n" // 5 include paths
	"\t\t</ClCompile>\n"
	"\t</ItemDefinitionGroup>\n"

	"\t<ItemGroup>\n"
	"\t\t<ClInclude Include=\"%ls.bootstrap.cpp\" />\n"
	"%ls"
	"\t</ItemGroup>\n"
	"\t<ItemGroup>\n"
	"\t\t<ClInclude Include=\"%ls.proxy.h\" />\n"
	"%ls"
	"\t</ItemGroup>\n"
	"\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n"
	"\t<ImportGroup Label=\"ExtensionTargets\" >\n"
	"\t</ImportGroup>\n"
	"</Project>",
	*identification,
	*moduleName,
	*moduleName,
	*moduleName,
	*moduleName,
	*defineName,
	*defines,
	*GetApplication<BuildSystemApplication>()->GetApplicationCodeName(),
	*includePath,
	*defineName,
	*defines,
	*GetApplication<BuildSystemApplication>()->GetApplicationCodeName(),
	*includePath,
	*moduleName,
	*compileList,
	*moduleName,
	*includeList
	);

	return true;
}

bool Module::GenerateProxyHeader(String* output)
{
	if (!output)
		return false;

	BuildSystem* bs = &GetApplication<BuildSystemApplication>()->GetBuildSystem();

	String defineLists;
	for (auto& module : *bs->GetModules())
	{
		//if (module.moduleName == moduleName)
		//{
		//	defineLists = String::Format("%ls#define %ls_API %ls\n", *defineLists, *module.defineName, "DLLEXPORT");
		//	continue;
		//}

		defineLists = String::Format("%ls#ifdef %ls_EXPORTS\n#define %ls_API __declspec(dllexport)\n#else\n#define %ls_API __declspec(dllimport)\n#endif\n\n", 
			*defineLists, *module.defineName, *module.defineName, *module.defineName);
	}

	*output = String::Format(
	"/* Copyright 2020 - 2025, Hansson Software. All rights reserved. Automatically generated by MeteorBuild(R), edit the corresponding .mrbuild file instead! */\n"
	"#pragma once\n#include \"%ls/%lsDefs.h\"\n\n"
	"%ls"
	//"#ifdef %ls_EXPORTS\n"
	//"#define %ls_API __declspec(dllexport)\n"
	//"#else\n"
	//"#define %ls_API __declspec(dllimport)\n"
	//"#endif\n"
	,
	*GetApplication<BuildSystemApplication>()->GetApplicationCodeName(),
	*GetApplication<BuildSystemApplication>()->GetApplicationCodeName(),
	*defineLists
		);

	return true;
}

bool Module::GenerateBootstrapHeader(String* output)
{
	*output =
		"/* Copyright 2020 - 2025, Hansson Software. All rights reserved. Automatically generated by MeteorBuild(R), edit the corresponding .mrbuild file instead! */\n\n"
		"#pragma once\n\n#include <Windows.h>\n"
		"BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)\n"
		"{\n"
		"\tswitch (fdwReason)\n"
		"\t{\n"
		"\t\tcase DLL_PROCESS_ATTACH:\n"
		"\t\t\tbreak;\n"
		"\t\tcase DLL_PROCESS_DETACH:\n"
		"\t\t\tbreak;\n"
		"\t\tcase DLL_THREAD_ATTACH:\n"
		"\t\t\tbreak;\n"
		"\t\tcase DLL_THREAD_DETACH:\n"
		"\t\t\tbreak;\n"
		"\t}\n\n"
		"\treturn 0;\n"
		"}\n";

	return true;
}
