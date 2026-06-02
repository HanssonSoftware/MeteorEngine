/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Commands.h"
#include "CommandRegistry.h"
#include "VisualStudioBoilerplates.h"

#include <Commandlet.h>
#include <Application/Application.h>
#include <Platform/Winapi.h>

#include <shellapi.h>
#include <Shlwapi.h>
#include <Shlobj.h>
#include <PathCch.h>

#pragma warning(disable : 6387)
#pragma warning(disable : 6031)
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Shell32.lib")

LOG_ADDCATEGORY(Commands);
LOG_ADDCATEGORY(Build);
LOG_ADDCATEGORY(ContentFinder);
LOG_ADDCATEGORY(FileConstruct);

namespace Commands
{
	ADD_NEW_BUILD_COMMAND("-make", "Generates project files", Generate_Cmd);

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
		static MemoryBlockArena<wchar_t> arena = { 32_mB };
#else
		static MemoryBlockArena<char> arena = { 16_mB };
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

			MR_LOG(LogContentFinder, Log, "Found %d files in folder: %ls", files.GetSize(), sourceDirectoryW);

			// Read scripts
			MemoryBlockArena<char> currentReadFile = { 4_mB };

			Array<Module> modules;
			for (auto& file : files)
			{
				wchar_t* extension = PathFindExtensionW(file);
				if (*extension && wcscmp(extension, L".mrbuild") != 0)
					continue;

				MR_LOG(LogContentFinder, Log, "Opening %ls script", file);

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
						MR_LOG(LogContentFinder, Error, "Failed to parse %ls script", file);
						MR_LOG(LogCommands, Error, "ReadFile returned: %s", *GetLastErrorString(), ::GetLastError());
					}

					CloseHandle(script);
					currentReadFile.Reset();
				}
				else
				{
					MR_LOG(LogContentFinder, Error, "Unable to open %ls script", file);
					MR_LOG(LogCommands, Error, "CreateFileW returned: %s", GetLastErrorString().Chr());
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
			
			const u32 required = swprintf(nullptr, 0, L"%s\\%hs", intermediateDirectoryW, GetApplication()->GetApplicationCodeName()) + 1;

			wchar_t* codeNameAppendedIntermediatePath = (wchar_t*)arena.Allocate(required * sizeof(wchar_t));
			swprintf(codeNameAppendedIntermediatePath, required, L"%s\\%hs", intermediateDirectoryW, GetApplication()->GetApplicationCodeName());

			CreateDirectoryW(codeNameAppendedIntermediatePath, nullptr);

			static MemoryBlockArena<char> moduleBuffer = { 1_mB };

			u32 filesCreated = 0;
			for (Module& mdl : modules)
			{
				u32 requiredEachModule = swprintf(nullptr, 0, L"%s\\%hs", codeNameAppendedIntermediatePath, mdl.moduleName.Chr()) + 1;
				wchar_t* fullPathToModuleFolder = (wchar_t*)moduleBuffer.Allocate(requiredEachModule * sizeof(wchar_t));
				swprintf(fullPathToModuleFolder, requiredEachModule, L"%s\\%hs", codeNameAppendedIntermediatePath, mdl.moduleName.Chr());

				CreateDirectoryW(fullPathToModuleFolder, nullptr);

				GUID id = {};
				CoCreateGuid(&id);

				wchar_t idStr[48] = {};
				StringFromGUID2(id, idStr, 48);
				WideCharToMultiByte(CP_UTF8, 0, idStr, 48, mdl.guid, 48, nullptr, nullptr);


				requiredEachModule = swprintf(nullptr, 0, L"%s\\%hs.vcxproj", fullPathToModuleFolder, mdl.moduleName.Chr()) + 1;
		 
				wchar_t* fullPathToIntermediate = (wchar_t*)moduleBuffer.Allocate(requiredEachModule * sizeof(wchar_t));
				swprintf(fullPathToIntermediate, L"%s\\%hs.vcxproj", fullPathToModuleFolder, mdl.moduleName.Chr());

				MR_LOG(LogFileConstruct, Log, "Creating %s project file", mdl.moduleName.Chr());

				HANDLE proj = CreateFileW(fullPathToIntermediate, GENERIC_READ | GENERIC_WRITE, bIsRunningDebugMode ? FILE_SHARE_READ : 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
				if (proj != INVALID_HANDLE_VALUE)
				{
					MemoryBlockArena<char> xmlAssembly = { 1_mB };
					DWORD written = 0;
					
					WriteFile(proj, Project::Configurations, (sizeof(Project::Configurations) / sizeof(Project::Configurations[0])) - 1, &written, nullptr);

					u32 requiredBytesForIdentificationPart = (sizeof(Project::ProjectIdentification) / sizeof(Project::ProjectIdentification[0])) + 64;
					char* identificationPartAllocatedGuid = (char*)xmlAssembly.Allocate(requiredBytesForIdentificationPart);

					requiredBytesForIdentificationPart = snprintf(identificationPartAllocatedGuid, requiredBytesForIdentificationPart, Project::ProjectIdentification, mdl.guid);

					WriteFile(proj, identificationPartAllocatedGuid, requiredBytesForIdentificationPart, &written, nullptr);

#ifdef MR_DEBUG
					FlushFileBuffers(proj);
#endif // MR_DEBUG

					WriteFile(proj, Project::PropertyGroups, (sizeof(Project::PropertyGroups) / sizeof(Project::PropertyGroups[0])) - 1, &written, nullptr);

					WriteFile(proj, Project::Macros, (sizeof(Project::Macros) / sizeof(Project::Macros[0])) - 1, &written, nullptr);


					u32 requiredBytesForConfigurationSettings = (sizeof(Project::ConfigurationDependentSettings) / sizeof(Project::ConfigurationDependentSettings[0])) + 64;
					char* allocatedForConfigSettings = (char*)xmlAssembly.Allocate(requiredBytesForConfigurationSettings);

					requiredBytesForConfigurationSettings = snprintf(allocatedForConfigSettings, requiredBytesForConfigurationSettings, Project::ConfigurationDependentSettings, 
						mdl.moduleName.Chr() /* <NMakeOutput>%s.dll</NMakeOutput> */, 
						"" /**/, 
						mdl.moduleName.Chr() /* <NMakeOutput>%s.dll</NMakeOutput> */,
						"" /**/, 
						mdl.moduleName.Chr() /* <NMakeOutput>%s.dll</NMakeOutput> */,
						"" /**/
					);

					WriteFile(proj, allocatedForConfigSettings, requiredBytesForConfigurationSettings, &written, nullptr);
#ifdef MR_DEBUG
					FlushFileBuffers(proj);
#endif // MR_DEBUG

					MemoryBlockArena<char> projectFilesIncluded = { 256_kB };
					for (const wchar_t* file : mdl.files)
					{
						char extension[16] = {};
						wchar_t* extensionType = PathFindExtensionW(file);

						if (!WideCharToMultiByte(CP_UTF8, 0, extensionType, wcslen(extensionType), extension, 16, nullptr, nullptr))
						{
							MR_LOG(LogBuild, Error, "WideCharToMultiByte returned: %s", GetLastErrorString().Chr());
							
							projectFilesIncluded.Reset();
							continue;
						}

						wchar_t pathTo[MAX_PATH] = {};
						PathRelativePathToW(pathTo, fullPathToIntermediate, FILE_ATTRIBUTE_NORMAL, file, FILE_ATTRIBUTE_NORMAL);

						u32 requiredBytesForAnEntry = snprintf(nullptr, 0, Project::EntryFormat(Hash(extension)), pathTo) + 1;

						char* xmlEntry = (char*)projectFilesIncluded.Allocate(requiredBytesForAnEntry);
						snprintf(xmlEntry, requiredBytesForAnEntry, Project::EntryFormat(Hash(extension)), pathTo);

						WriteFile(proj, xmlEntry, requiredBytesForAnEntry - 1, &written, nullptr);

//#ifdef MR_DEBUG
//						FlushFileBuffers(proj); // Forget it
//#endif // MR_DEBUG
					}

					WriteFile(proj, Project::BottomPart, (sizeof(Project::BottomPart) / sizeof(Project::BottomPart[0])) - 1, &written, nullptr);

					CloseHandle(proj);
					filesCreated++;
				}
				else
				{
					MR_LOG(LogFileConstruct, Error, "Failed to create module file asset %s", mdl.moduleName.Chr());
					MR_LOG(LogFileConstruct, Error, "CreateFileW failed with: %s", GetLastErrorString().Chr());
				}

				moduleBuffer.Reset();
			}

			if (filesCreated == modules.GetSize())
			{
				MR_LOG(LogFileConstruct, Log, "Successfully generated all project files");
				AddSolution(&modules);
			}
			else if (filesCreated != modules.GetSize())
			{
				MR_LOG(LogFileConstruct, Log, "Successfully made %d modules from %d", filesCreated, modules.GetSize());
				AddSolution(&modules);
			}
			else
			{
				MR_LOG(LogFileConstruct, Warn, "Something went wrong at generating project files from modules, investigate immediately!");
			}
		}

		QueryPerformanceCounter(&endTime);
		QueryPerformanceFrequency(&frequency);

		MR_LOG(LogCommands, Log, "Generate command ran in %.3f seconds", ((double)endTime.QuadPart - (double)startTime.QuadPart) / (double)frequency.QuadPart);
	}

	void AddSolution(const Array<Module>* modules)
	{
		const String path = Commandlet::Get().Check("-sln") ? Commandlet::Get().Parse("-sln") : Commandlet::Get().Parse("-s");
		if (path.IsEmpty())
		{
			MR_LOG(LogFileConstruct, Error, "Failed to find source/sln parameter!");
			return;
		}

		MemoryBlockArena<wchar_t> buffer = { 1_mB };

		wchar_t* convertedPath = (wchar_t*)buffer.Allocate(path.Length() * sizeof(wchar_t));
		MultiByteToWideChar(CP_UTF8, 0, path.Chr(), path.Length(), convertedPath, (path.Length() + 1) * sizeof(char));

		if (PathIsRelativeW(convertedPath))
		{
			wchar_t exeLocation[MAX_PATH] = {};

			wcscpy_s(exeLocation, MAX_PATH, Commandlet::Get().GetArgs()[0]);
			PathRemoveFileSpecW(exeLocation);

			const u32 combinedDirectorySize = (u32)wcslen(exeLocation) + (u32)wcslen(convertedPath);
			wchar_t* newDirectory = (wchar_t*)buffer.Allocate((combinedDirectorySize + 10) * sizeof(wchar_t));

			PathCchCombine(newDirectory, combinedDirectorySize, exeLocation, convertedPath);
			convertedPath = newDirectory;
		}

		PathCchRemoveFileSpec(convertedPath, wcslen(convertedPath));

		const u32 requiredFormatAmount = swprintf(nullptr, 0, L"%s\\%hs.sln", convertedPath, (*modules)[0].parent.Chr()) + 1;

		wchar_t* fullPathToSln = (wchar_t*)buffer.Allocate(requiredFormatAmount * sizeof(wchar_t));
		swprintf(fullPathToSln, requiredFormatAmount, L"%s\\%hs.sln", convertedPath, (*modules)[0].parent.Chr());

		MR_LOG(LogFileConstruct, Log, "Creating %s solution file", (*modules)[0].parent.Chr());

		HANDLE slnx = CreateFileW(fullPathToSln, GENERIC_READ | GENERIC_WRITE, bIsRunningDebugMode ? FILE_SHARE_READ : 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (slnx != INVALID_HANDLE_VALUE)
		{
			MemoryBlockArena<char> outputFileBuffer = { 8_mB };
			MemoryBlockArena<wchar_t> pathCreating = { 1_mB };
			
			const String intermediateDir = Commandlet::Get().Parse("-i");

			wchar_t* intermediateDirectoryW = (wchar_t*)pathCreating.Allocate(intermediateDir.Length() * sizeof(wchar_t));
			if (!MultiByteToWideChar(CP_UTF8, 0, intermediateDir, intermediateDir.Length(), intermediateDirectoryW, intermediateDir.Length()))
			{
				MR_LOG(LogCommands, Error, "%s", *GetLastErrorString());
				return;
			}

			const u32 appCodeNameCount = strlen(GetApplication()->GetApplicationCodeName());
			const u32 sumOfPaths = wcslen(intermediateDirectoryW) + appCodeNameCount;
			wchar_t* pathToProductName = (wchar_t*)pathCreating.Allocate(sumOfPaths);

			wchar_t codeName[16] = {};
			MultiByteToWideChar(CP_UTF8, 0, GetApplication()->GetApplicationCodeName(), appCodeNameCount, codeName, appCodeNameCount);

			if (PathCchCombine(pathToProductName, sumOfPaths + 16, intermediateDirectoryW, codeName) != S_OK)
			{
				MR_LOG(LogFileConstruct, Fatal, "PathCchCombine error!");
				return;
			}

			DWORD written = 0;
			WriteFile(slnx, Solution::Header, (sizeof(Solution::Header) / sizeof(Solution::Header[0])) - 1, &written, nullptr);

			for (auto& mdl : *modules)
			{
				const u32 requiredAmountToFormat = snprintf(nullptr, 0, "\nProject(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s\", \"%ls\\%s\\%s.vcxproj\", \"%s\"\nEndProject", mdl.moduleName.Chr(), pathToProductName, mdl.moduleName.Chr(), mdl.moduleName.Chr(), mdl.guid) + 1;
				char* lineBuffer = (char*)outputFileBuffer.Allocate(requiredAmountToFormat);

				snprintf(lineBuffer, requiredAmountToFormat, "\nProject(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s\", \"%ls\\%s\\%s.vcxproj\", \"%s\"\nEndProject", mdl.moduleName.Chr(), pathToProductName, mdl.moduleName.Chr(), mdl.moduleName.Chr(), mdl.guid);

				WriteFile(slnx, lineBuffer, requiredAmountToFormat - 1, &written, nullptr);

#ifdef MR_DEBUG
				if (written > 256) FlushFileBuffers(slnx);
#endif // MR_DEBUG
			}

			constexpr const char preSolutions[] =
				"\nGlobal\n"
				"	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n"
				"		Debug|x64 = Debug|x64\n"
				"		Development|x64 = Development|x64\n"
				"		Shipping|x64 = Shipping|x64\n"
				"	EndGlobalSection\n";


			WriteFile(slnx, preSolutions, (sizeof(preSolutions) / sizeof(preSolutions[0])) - 1, &written, nullptr);

			constexpr const char openProjectConfigurationPlatforms[] = "	GlobalSection(ProjectConfigurationPlatforms) = postSolution";
			WriteFile(slnx, openProjectConfigurationPlatforms, (sizeof(openProjectConfigurationPlatforms) / sizeof(openProjectConfigurationPlatforms[0])) - 1, &written, nullptr);

			for (auto& mdl : *modules)
			{
				constexpr const char activeCfgTemplate[] =
					"\n\t\t%s.Debug|x64.ActiveCfg = Debug|x64"
					"\n\t\t%s.Debug|x64.Build.0 = Debug|x64"
					"\n\t\t%s.Development|x64.ActiveCfg = Development|x64"
					"\n\t\t%s.Development|x64.Build.0 = Development|x64"
					"\n\t\t%s.Shipping|x64.ActiveCfg = Shipping|x64"
					"\n\t\t%s.Shipping|x64.Build.0 = Shipping|x64";

				const u32 requiredAmountToFormat = snprintf(nullptr, 0, activeCfgTemplate, mdl.guid, mdl.guid, mdl.guid, mdl.guid, mdl.guid, mdl.guid) + 1;
				char* lineBuffer = (char*)outputFileBuffer.Allocate(requiredAmountToFormat);

				snprintf(lineBuffer, requiredAmountToFormat, activeCfgTemplate, mdl.guid, mdl.guid, mdl.guid, mdl.guid, mdl.guid, mdl.guid);

				WriteFile(slnx, lineBuffer, requiredAmountToFormat - 1, &written, nullptr);

#ifdef MR_DEBUG
				FlushFileBuffers(slnx);
#endif // MR_DEBUG
			}

			constexpr const char closeProjectConfigurationPlatforms[] = "\n\tEndGlobalSection\n";
			WriteFile(slnx, closeProjectConfigurationPlatforms, (sizeof(closeProjectConfigurationPlatforms) / sizeof(closeProjectConfigurationPlatforms[0])) - 1, &written, nullptr);

			GUID id = {};
			CoCreateGuid(&id);

			wchar_t idStr[48] = {};
			StringFromGUID2(id, idStr, 48);

			constexpr const char finalSolutionProperties[] =
				"	GlobalSection(SolutionProperties) = preSolution\n"
				"		HideSolutionNode = FALSE\n"
				"	EndGlobalSection\n"
				"	GlobalSection(ExtensibilityGlobals) = postSolution\n"
				"		SolutionGuid = %ls\n"
				"	EndGlobalSection\n"
				"EndProject\n";

			const u32 requiredAmountToFormat = snprintf(nullptr, 0, finalSolutionProperties, idStr) + 1;
			char* lineBuffer = (char*)outputFileBuffer.Allocate(requiredAmountToFormat);

			snprintf(lineBuffer, requiredAmountToFormat, finalSolutionProperties, idStr);

			WriteFile(slnx, lineBuffer, requiredAmountToFormat - 1, &written, nullptr);
			CloseHandle(slnx);
		}
		else
		{
			MR_LOG(LogFileConstruct, Error, "Failed to create finalization file for: %s", (*modules)[0].parent.Chr());
			MR_LOG(LogFileConstruct, Error, "CreateFileW failed with: %s", GetLastErrorString().Chr());
		}

		if (Commandlet::Get().Check("-o"))
		{
			MR_LOG(LogBuild, Log, "Opening trophy: %ls", fullPathToSln);
			ShellExecuteW(nullptr, L"open", fullPathToSln, nullptr, nullptr, SW_SHOWNORMAL);
		}
	}
}
