/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Commands.h"
#include "CommandRegistry.h"

#include <Commandlet.h>
#include "Module.h"

#include <Platform/Winapi.h>
#include <Shlwapi.h>
#include <PathCch.h>
#include <Application/Application.h>

#pragma warning(disable : 6387)
#pragma warning(disable : 6031)

LOG_ADDCATEGORY(Commands);
LOG_ADDCATEGORY(Build);
LOG_ADDCATEGORY(Validator);

namespace Commands
{
	ADD_NEW_BUILD_COMMAND("-make", "Generates project files", Generate_Cmd);

	inline constexpr const char* InsertFormatSpecifier()
	{
		constexpr const char* vcxprojTemplate = R"XML(<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
<ItemGroup Label="ProjectConfigurations">
<ProjectConfiguration Include="Debug|x64">
 <Configuration>Debug</Configuration>
 <Platform>x64</Platform>
</ProjectConfiguration>
 <ProjectConfiguration Include="Development|x64">
 <Configuration>Development</Configuration>
 <Platform>x64</Platform>
</ProjectConfiguration>
 <ProjectConfiguration Include="Shipping|x64">
 <Configuration>Shipping</Configuration>
 <Platform>x64</Platform>
</ProjectConfiguration>
</ItemGroup>
<PropertyGroup Label="Globals">
<VCProjectVersion>18.0</VCProjectVersion>
<ProjectGuid>%s</ProjectGuid>
<Keyword>Win32Proj</Keyword>
</PropertyGroup>
<Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
<ConfigurationType>DynamicLibrary</ConfigurationType>
<UseDebugLibraries>true</UseDebugLibraries>
<PlatformToolset>v145</PlatformToolset>
</PropertyGroup>
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Development|x64'" Label="Configuration">
<ConfigurationType>DynamicLibrary</ConfigurationType>
<UseDebugLibraries>true</UseDebugLibraries>
<PlatformToolset>v145</PlatformToolset>
</PropertyGroup>
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Shipping|x64'" Label="Configuration">
<ConfigurationType>DynamicLibrary</ConfigurationType>
<UseDebugLibraries>false</UseDebugLibraries>
<PlatformToolset>v145</PlatformToolset>
</PropertyGroup>
<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
<ImportGroup Label="ExtensionSettings">
</ImportGroup>
<ImportGroup Label="Shared">
</ImportGroup>
<ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
<Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
</ImportGroup>
<ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Development|x64'">
<Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
</ImportGroup>
<ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Shipping|x64'">
<Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
</ImportGroup>
<PropertyGroup Label="UserMacros" />
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
<NMakeBuildCommandLine>$(ProductDir)MeteorBuild.exe -build -c Debug</NMakeBuildCommandLine>
<NMakeOutput>%s.dll</NMakeOutput>
<NMakePreprocessorDefinitions>MR_DEBUG;$(NMakePreprocessorDefinitions)</NMakePreprocessorDefinitions>
<NMakeReBuildCommandLine>gvhjk</NMakeReBuildCommandLine>
<NMakeCleanCommandLine>hjkv</NMakeCleanCommandLine>
<NMakeIncludeSearchPath>hjvk</NMakeIncludeSearchPath>
<NMakeForcedIncludes>jkghj</NMakeForcedIncludes>
<NMakeAssemblySearchPath>vhjk</NMakeAssemblySearchPath>
<NMakeForcedUsingAssemblies>gjh</NMakeForcedUsingAssemblies>
<AdditionalOptions>,n</AdditionalOptions>
</PropertyGroup>
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Development|x64'">
<NMakeBuildCommandLine>gch</NMakeBuildCommandLine>
<NMakeOutput>%s.dll</NMakeOutput>
<NMakePreprocessorDefinitions>MR_DEBUG;$(NMakePreprocessorDefinitions)</NMakePreprocessorDefinitions>
</PropertyGroup>
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Shipping|x64'">
<NMakeBuildCommandLine>gch</NMakeBuildCommandLine>
<NMakeOutput>%s.dll</NMakeOutput>
<NMakePreprocessorDefinitions>$(NMakePreprocessorDefinitions)</NMakePreprocessorDefinitions>
</PropertyGroup>
<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
<NMakeCompile>
	<NMakeCompileFileCommandLine>nm,</NMakeCompileFileCommandLine>
</NMakeCompile>
</ItemDefinitionGroup>
<ItemGroup>
</ItemGroup>
<Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
<ImportGroup Label="ExtensionTargets">
</ImportGroup>
</Project>)XML";

		return vcxprojTemplate;
	}

	void Generate_Cmd()
	{
		LARGE_INTEGER startTime, endTime, frequency;
		QueryPerformanceCounter(&startTime);

		const String sourceDirectory = Commandlet::Get().Parse("-s");
		const String intermediateDirectory = Commandlet::Get().Parse("-i");

		// Arena allocates bytes, so equal size will be allocated!
		// WChar: 32 MB (Char / 2)
		// Char: 32 MB

#ifdef MR_PLATFORM_WINDOWS
		static MemoryBlockArena<wchar_t> arena = { 16 * 1024 * 1024 };
#else
		static MemoryBlockArena<char> arena = { 16 * 1024 * 1024 };
#endif // MR_PLATFORM_WINDOWS

		if (sourceDirectory && intermediateDirectory)
		{
			wchar_t* sourceDirectoryW = (wchar_t*)arena.Allocate(sourceDirectory.Length() * sizeof(wchar_t));
			if (!MultiByteToWideChar(CP_UTF8, 0, sourceDirectory, sourceDirectory.Length(), sourceDirectoryW, sourceDirectory.Length()))
			{
				MR_LOG(LogCommands, Error, "%s", *GetLastErrorString());
				return;
			}

			// Collect files in folder

			Array<wchar_t*> files;
			DirectorySearch(sourceDirectoryW, files, &arena);

			if (files.GetSize() < 0)
				return;

			// Read scripts
			MemoryBlockArena<char> currentReadFile = { 8 * 1024 * 1024 };

			Array<Module> modules;
			for (auto& file : files)
			{
				wchar_t* extension = PathFindExtensionW(file);
				if (*extension && wcscmp(extension, L".mrbuild") != 0)
					continue;

				HANDLE script = CreateFileW(file, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
				if (script != INVALID_HANDLE_VALUE)
				{
					LARGE_INTEGER size;
					GetFileSizeEx(script, &size);

					char* allocatedBufferForScript = (char*)currentReadFile.Allocate(size.QuadPart);

					DWORD actualRead = 0;
					if (ReadFile(script, allocatedBufferForScript, (DWORD)size.QuadPart, &actualRead, nullptr))
					{
						Module module = Module::MakeModuleFromBuffer(allocatedBufferForScript);

						u32 wideNameSize = (u32)wcslen(file);
						char* narrowName = (char*)arena.Allocate(wideNameSize + 1);

						WideCharToMultiByte(CP_UTF8, 0, file, wideNameSize, narrowName, wideNameSize, nullptr, 0);
						module.path = narrowName;

						DirectorySearch(file, module.files, &arena);

						modules.Add(module);
					}
					else
					{
						MR_LOG(LogCommands, Error, "%s (%d)", *GetLastErrorString(), ::GetLastError());
					}

					CloseHandle(script);
					currentReadFile.Reset();
				}
				else
				{
					MR_LOG(LogCommands, Error, "%s", *GetLastErrorString());
				}
			}

			wchar_t* intermediateDirectoryW = (wchar_t*)arena.Allocate(intermediateDirectory.Length() * sizeof(wchar_t));
			if (!MultiByteToWideChar(CP_UTF8, 0, intermediateDirectory, intermediateDirectory.Length(), intermediateDirectoryW, intermediateDirectory.Length()))
			{
				MR_LOG(LogCommands, Error, "%s", *GetLastErrorString());
				return;
			}

			if (PathIsRelativeW(intermediateDirectoryW))
			{
				wchar_t exeLocation[MAX_PATH] = {};

				wcscpy_s(exeLocation, MAX_PATH, Commandlet::Get().GetArgs()[0]);
				PathRemoveFileSpecW(exeLocation);

				const u32 combinedDirectorySize = (u32)wcslen(exeLocation) + (u32)wcslen(intermediateDirectoryW);
				wchar_t* newDirectory = (wchar_t*)arena.Allocate((combinedDirectorySize + 10) * sizeof(wchar_t));

				PathCchCombine(newDirectory, combinedDirectorySize, exeLocation, intermediateDirectoryW);
				intermediateDirectoryW = newDirectory;
			}
			
			static MemoryBlockArena<char> moduleBuffer = { 1_mB };
			for (Module& mdl : modules)
			{
				GUID id = {};
				CoCreateGuid(&id);

				wchar_t idStr[48] = {};
				StringFromGUID2(id, idStr, 48);
				WideCharToMultiByte(CP_UTF8, 0, idStr, 48, mdl.guid, 48, nullptr, nullptr);

				wchar_t* fullPathToIntermediate = (wchar_t*)moduleBuffer.Allocate((wcslen(intermediateDirectoryW) + strlen(GetApplication()->GetApplicationCodeName()) + (mdl.moduleName.Length() * 2)) * sizeof(wchar_t));
				swprintf(fullPathToIntermediate, L"%s\\%hs\\%hs\\%hs.vcxproj", intermediateDirectoryW, GetApplication()->GetApplicationCodeName(), *mdl.moduleName, *mdl.moduleName);

				HANDLE proj = CreateFileW(fullPathToIntermediate, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
				if (proj != INVALID_HANDLE_VALUE)
				{
					const u32 required = (u32)snprintf(nullptr, 0, InsertFormatSpecifier(), mdl.guid, mdl.moduleName.Chr(), mdl.moduleName.Chr(), mdl.moduleName.Chr());
					char* writeOut = (char*)moduleBuffer.Allocate(required + 1);

					snprintf(writeOut, required + 1, InsertFormatSpecifier(), mdl.guid, mdl.moduleName.Chr(), mdl.moduleName.Chr(), mdl.moduleName.Chr());
					
					DWORD written = 0;
					WriteFile(proj, writeOut, required + 1, &written, nullptr);
					CloseHandle(proj);
				}

				moduleBuffer.Reset();
			}


			int J = 43;
		}

		QueryPerformanceCounter(&endTime);
		QueryPerformanceFrequency(&frequency);

		MR_LOG(LogCommands, Log, "Build command ran in %.4f seconds!", (endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart);
	}
}
