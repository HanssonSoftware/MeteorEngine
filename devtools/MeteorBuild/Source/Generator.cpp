/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Commands.h"
#include "CommandRegistry.h"
#include "VisualStudio/Boilerplates.h"
#include <Memory/MemoryBlockArena.h>

#include <HAL/Commandline.h>
#include <Application/Application.h>
#include <HAL/Timer.h>
#include <HAL/File.h>
#include "Win32/MinimalWin.h"

#include <shellapi.h>
#include <Shlwapi.h>
#include <Shlobj.h>
#include <PathCch.h>

#pragma warning(disable : 6387)
#pragma warning(disable : 6031)
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Shell32.lib")

LOG_ADDCATEGORY(Factory);

namespace Commands
{
	ADD_NEW_BUILD_COMMAND("-make", "Generates project files", Generate_Cmd);

	void VcxprojGenerate(Module& mdl, const wchar_t* fullPathToIntermediateDir, u32& createdFilesCount);

	bool Generate_Cmd()
	{
		constexpr u64 maxChars = 16_mB / sizeof(wchar_t);

		wchar_t* allocated = GetMemoryManager()->Allocate<wchar_t>(16_mB);
		if (!allocated)
			return false;

		const Commandline* cli = GetApplication()->GetCommandline();

		const StringView sourceDirectoryFromParameter = cli->Get("-s"); // ..\\engine
		if (!sourceDirectoryFromParameter.ptr)
		{
			MR_LOG(LogFactory, Error, "Source directory parameter missing!");
			return false;
		}

		u32 exeLocationCount = (DWORD)GetModuleFileNameW(nullptr, allocated, maxChars);

		for (wchar_t* p = &allocated[exeLocationCount]; *p != L'\\'; p--)
			*p = L'\0';

		exeLocationCount = (u32)wcslen(allocated);

		exeLocationCount += MultiByteToWideChar(CP_UTF8, 0, (char*)sourceDirectoryFromParameter.ptr, sourceDirectoryFromParameter.size,
			&allocated[exeLocationCount], maxChars - exeLocationCount);
		if (GetLastError())
		{
			MR_LOG(LogFactory, Fatal, "Failed to convert! MultiByteToWideChar=%d", GetLastError());
			return false;
		}

		MemoryBlockArena* discoveryArena = GetMemoryManager()->RequestNewRegion<MemoryBlockArena>("discoveryArena", 16_mB * sizeof(wchar_t));
		Array<wchar_t*> files;
		DirectorySearch(allocated, exeLocationCount, discoveryArena, files);
		
		Array<Module> modules;
		MemoryBlockArena* processingArena = GetMemoryManager()->RequestNewRegion<MemoryBlockArena>("processingArena", 8_mB * sizeof(wchar_t));
		for (wchar_t* file : files)
		{
			if (wcscmp(PathFindExtensionW(file), L".mrbuild"))
				continue;

			HANDLE scriptFile = CreateFileW(file, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (scriptFile != INVALID_HANDLE_VALUE)
			{
				LARGE_INTEGER lg;
				GetFileSizeEx(scriptFile, &lg);

				char* buffer = (char*)processingArena->Allocate(lg.QuadPart);

				DWORD read = 0;
				if (!ReadFile(scriptFile, buffer, lg.QuadPart, &read, nullptr))
				{
					CloseHandle(scriptFile);
					continue;
				}

				Module instance = MakeModuleFromBuffer(buffer);
				

				modules.Add(instance);
				CloseHandle(scriptFile);
			}
		}

		if (modules.GetSize() == 0)
		{
			MR_LOG(LogFactory, Error, "No modules created!");
			return false;
		}

		const StringView intermediateDirectory = cli->Get("-i");
		
//
//		// Arena allocates bytes, so equal size will be allocated!
//		// WChar: 32 MB (Char / 2)
//		// Char: 32 MB
//
//#ifdef MR_PLATFORM_WINDOWS
//		static MemoryBlockArena<wchar_t> arena = { 32_mB };
//#else
//		static MemoryBlockArena<char> arena = { 16_mB };
//#endif // MR_PLATFORM_WINDOWS
//
//		if (sourceDirectory && intermediateDirectory)
//		{
//			wchar_t* sourceDirectoryW = (wchar_t*)arena.Allocate(sourceDirectory.Length() * sizeof(wchar_t));
//			if (!MultiByteToWideChar(CP_UTF8, 0, sourceDirectory, sourceDirectory.Length(), sourceDirectoryW, sourceDirectory.Length()))
//			{
//				MR_LOG(LogCommands, Error, "%s", *GetLastErrorString());
//				return;
//			}
//
//			// Collect files in folder
//
//			Array<wchar_t*> files;
//			DirectorySearch(sourceDirectoryW, files, &arena);
//
//			if (files.GetSize() < 0)
//				return;
//
//			MR_LOG(LogContentFinder, Log, "Found %d files in folder: %ls", files.GetSize(), sourceDirectoryW);
//
//			// Read scripts
//			MemoryBlockArena<char> currentReadFile = { 4_mB };
//
//			Array<IModule> modules;
//			for (auto& file : files)
//			{
//				wchar_t* extension = PathFindExtensionW(file);
//				if (*extension && wcscmp(extension, L".mrbuild") != 0)
//					continue;
//
//				MR_LOG(LogContentFinder, Log, "Opening %ls script", file);
//
//				HANDLE script = SendToOutputBuffer(file, GENERIC_READ, _placeholder_);
//				if (script != INVALID_HANDLE_VALUE)
//				{
//					LARGE_INTEGER size;
//					GetFileSizeEx(script, &size);
//
//					char* allocatedBufferForScript = (char*)currentReadFile.Allocate(size.QuadPart);
//
//					DWORD actualRead = 0;
//					if (ReadFile(script, allocatedBufferForScript, (DWORD)size.QuadPart, &actualRead, nullptr))
//					{
//						IModule module = IModule::MakeModuleFromBuffer(allocatedBufferForScript);
//
//						u32 wideNameSize = (u32)wcslen(file);
//						char* narrowName = (char*)arena.Allocate(wideNameSize + 1);
//
//						WideCharToMultiByte(CP_UTF8, 0, file, wideNameSize, narrowName, wideNameSize, nullptr, 0);
//						module.path = narrowName;
//
//						DirectorySearch(file, module.files, &arena);
//
//						modules.Add(module);
//					}
//					else
//					{
//						MR_LOG(LogContentFinder, Error, "Failed to parse %ls script", file);
//						MR_LOG(LogCommands, Error, "ReadFile returned: %s", *GetLastErrorString(), ::GetLastError());
//					}
//
//					CloseHandle(script);
//					currentReadFile.Reset();
//				}
//				else
//				{
//					MR_LOG(LogContentFinder, Error, "Unable to open %ls script", file);
//					MR_LOG(LogCommands, Error, "CreateFileW returned: %s", GetLastErrorString().Chr());
//				}
//			}
//
//			wchar_t* intermediateDirectoryW = (wchar_t*)arena.Allocate(intermediateDirectory.Length() * sizeof(wchar_t));
//			if (!MultiByteToWideChar(CP_UTF8, 0, intermediateDirectory, intermediateDirectory.Length(), intermediateDirectoryW, intermediateDirectory.Length()))
//			{
//				MR_LOG(LogCommands, Error, "%s", *GetLastErrorString());
//				return;
//			}
//
//			if (PathIsRelativeW(intermediateDirectoryW))
//			{
//				wchar_t exeLocation[MAX_PATH] = {};
//
//				wcscpy_s(exeLocation, MAX_PATH, Commandline::Get().GetArgs()[0]);
//				PathRemoveFileSpecW(exeLocation);
//
//				const u32 combinedDirectorySize = (u32)wcslen(exeLocation) + (u32)wcslen(intermediateDirectoryW);
//				wchar_t* newDirectory = (wchar_t*)arena.Allocate((combinedDirectorySize + 10) * sizeof(wchar_t));
//
//				PathCchCombine(newDirectory, combinedDirectorySize, exeLocation, intermediateDirectoryW);
//				intermediateDirectoryW = newDirectory;
//			}
//			
//			const u32 required = swprintf(nullptr, 0, L"%s\\%hs", intermediateDirectoryW, GetApplication()->GetApplicationCodeName()) + 1;
//
//			wchar_t* codeNameAppendedIntermediatePath = (wchar_t*)arena.Allocate(required * sizeof(wchar_t));
//			swprintf(codeNameAppendedIntermediatePath, required, L"%s\\%hs", intermediateDirectoryW, GetApplication()->GetApplicationCodeName());
//
//			CreateDirectoryW(codeNameAppendedIntermediatePath, nullptr);
//
//			static MemoryBlockArena<char> moduleBuffer = { 1_mB };
//
//			u32 filesCreated = 0;
//			for (IModule& mdl : modules)
//			{
//				u32 requiredEachModule = swprintf(nullptr, 0, L"%s\\%hs", codeNameAppendedIntermediatePath, mdl.moduleName.Chr()) + 1;
//				wchar_t* fullPathToModuleFolder = (wchar_t*)moduleBuffer.Allocate(requiredEachModule * sizeof(wchar_t));
//				swprintf(fullPathToModuleFolder, requiredEachModule, L"%s\\%hs", codeNameAppendedIntermediatePath, mdl.moduleName.Chr());
//
//				CreateDirectoryW(fullPathToModuleFolder, nullptr);
//
//				GUID id = {};
//				CoCreateGuid(&id);
//
//				wchar_t idStr[48] = {};
//				StringFromGUID2(id, idStr, 48);
//				WideCharToMultiByte(CP_UTF8, 0, idStr, 48, mdl.guid, 48, nullptr, nullptr);
//
//
//				requiredEachModule = swprintf(nullptr, 0, L"%s\\%hs.vcxproj", fullPathToModuleFolder, mdl.moduleName.Chr()) + 1;
//
//				wchar_t* fullPathToIntermediate = (wchar_t*)moduleBuffer.Allocate(requiredEachModule * sizeof(wchar_t));
//				LogStandard(fullPathToIntermediate, L"%s\\%hs.vcxproj", fullPathToModuleFolder, mdl.moduleName.Chr());
//
//				VcxprojGenerate(mdl, fullPathToIntermediate, filesCreated);
//			}
//
//			if (filesCreated == modules.GetSize())
//			{
//				MR_LOG(LogFileConstruct, Log, "Successfully generated all project files");
//				AddSolution(&modules);
//			}
//			else if (filesCreated != modules.GetSize())
//			{
//				MR_LOG(LogFileConstruct, Log, "Successfully made %d modules from %d", filesCreated, modules.GetSize());
//				AddSolution(&modules);
//			}
//			else
//			{
//				MR_LOG(LogFileConstruct, Warn, "Something went wrong at generating project files from modules, investigate immediately!");
//			}
		//}

		//QueryPerformanceCounter(&endTime);
		//QueryPerformanceFrequency(&frequency);

		//MR_LOG(LogCommands, Log, "Generate command ran in %.3f seconds", ((double)endTime.QuadPart - (double)startTime.QuadPart) / (double)frequency.QuadPart);
	
		return true;
	}

	void AddSolution(const Array<Module>* modules)
	{
		//const String path = Commandline::Get().Check("-sln") ? Commandline::Get().Parse("-sln") : Commandline::Get().Parse("-s");
		//if (path.IsEmpty())
		{
			//MR_LOG(LogFileConstruct, Error, "Failed to find source/sln parameter!");
			return;
		}

		//MemoryBlockArena<wchar_t> buffer = { 1_mB };

		//wchar_t* convertedPath = (wchar_t*)buffer.Allocate(path.Length() * sizeof(wchar_t));
		//MultiByteToWideChar(CP_UTF8, 0, path.Chr(), path.Length(), convertedPath, (path.Length() + 1) * sizeof(char));
		//
		//if (PathIsRelativeW(convertedPath))
		//{
		//	wchar_t exeLocation[MAX_PATH] = {};
		//
		//	//wcscpy_s(exeLocation, MAX_PATH, Commandline::Get().GetArgs()[0]);
		//	PathRemoveFileSpecW(exeLocation);
		//
		//	const u32 combinedDirectorySize = (u32)wcslen(exeLocation) + (u32)wcslen(convertedPath);
		//	//wchar_t* newDirectory = (wchar_t*)buffer.Allocate((combinedDirectorySize + 10) * sizeof(wchar_t));
		//
		//	PathCchCombine(newDirectory, combinedDirectorySize, exeLocation, convertedPath);
		//	convertedPath = newDirectory;
		//}
		//
		//PathCchRemoveFileSpec(convertedPath, wcslen(convertedPath));
		//
		//const u32 requiredFormatAmount = swprintf(nullptr, 0, L"%s\\%hs.sln", convertedPath, (*modules)[0].parent.Chr()) + 1;
		//
		//wchar_t* fullPathToSln = (wchar_t*)buffer.Allocate(requiredFormatAmount * sizeof(wchar_t));
		//swprintf(fullPathToSln, requiredFormatAmount, L"%s\\%hs.sln", convertedPath, (*modules)[0].parent.Chr());
		//
		//MR_LOG(LogFileConstruct, Log, "Creating %s solution file", (*modules)[0].parent.Chr());

		//HANDLE slnx = CreateFileW(fullPathToSln, GENERIC_READ | GENERIC_WRITE, 0);
		//if (slnx != INVALID_HANDLE_VALUE)
		//{
			//MemoryBlockArena<char> outputFileBuffer = { 8_mB };
			//MemoryBlockArena<wchar_t> pathCreating = { 1_mB };
			
			//const String intermediateDir = Commandline::Get().Parse("-i");

			// wchar_t* intermediateDirectoryW = (wchar_t*)pathCreating.Allocate(intermediateDir.Length() * sizeof(wchar_t));
			// if (!MultiByteToWideChar(CP_UTF8, 0, intermediateDir, intermediateDir.Length(), intermediateDirectoryW, intermediateDir.Length()))
			{
				//MR_LOG(LogCommands, Error, "%s", *GetLastErrorString());
				return;
			}

			//const u32 appCodeNameCount = strlen(GetApplication()->GetApplicationCodeName());
			//const u32 sumOfPaths = wcslen(intermediateDirectoryW) + appCodeNameCount;
			//wchar_t* pathToProductName = (wchar_t*)pathCreating.Allocate(sumOfPaths);

			wchar_t codeName[16] = {};
			//MultiByteToWideChar(CP_UTF8, 0, GetApplication()->GetApplicationCodeName(), appCodeNameCount, codeName, appCodeNameCount);

			//if (PathCchCombine(pathToProductName, sumOfPaths + 16, intermediateDirectoryW, codeName) != S_OK)
			{
				//MR_LOG(LogFileConstruct, Fatal, "PathCchCombine error!");
				return;
			}

			DWORD written = 0;
			//WriteFile(slnx, Solution::Header, (sizeof(Solution::Header) / sizeof(Solution::Header[0])) - 1, &written, nullptr);

			for (auto& mdl : *modules)
			{
				//const u32 requiredAmountToFormat = snprintf(nullptr, 0, "\nProject(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s\", \"%ls\\%s\\%s.vcxproj\", \"%s\"\nEndProject", mdl.moduleName.Chr(), pathToProductName, mdl.moduleName.Chr(), mdl.moduleName.Chr(), mdl.guid) + 1;
				//char* lineBuffer = (char*)outputFileBuffer.Allocate(requiredAmountToFormat);

				//snprintf(lineBuffer, requiredAmountToFormat, "\nProject(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"%s\", \"%ls\\%s\\%s.vcxproj\", \"%s\"\nEndProject", mdl.moduleName.Chr(), pathToProductName, mdl.moduleName.Chr(), mdl.moduleName.Chr(), mdl.guid);

				//WriteFile(slnx, lineBuffer, requiredAmountToFormat - 1, &written, nullptr);

#ifdef MR_DEBUG
				//if (written > 256) FlushFileBuffers(slnx);
#endif // MR_DEBUG
			}

			constexpr const char preSolutions[] =
				"\nGlobal\n"
				"	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n"
				"		Debug|x64 = Debug|x64\n"
				"		Development|x64 = Development|x64\n"
				"		Shipping|x64 = Shipping|x64\n"
				"	EndGlobalSection\n";


			//WriteFile(slnx, preSolutions, (sizeof(preSolutions) / sizeof(preSolutions[0])) - 1, &written, nullptr);

			constexpr const char openProjectConfigurationPlatforms[] = "	GlobalSection(ProjectConfigurationPlatforms) = postSolution";
			//WriteFile(slnx, openProjectConfigurationPlatforms, (sizeof(openProjectConfigurationPlatforms) / sizeof(openProjectConfigurationPlatforms[0])) - 1, &written, nullptr);

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
				//char* lineBuffer = (char*)outputFileBuffer.Allocate(requiredAmountToFormat);

				// snprintf(lineBuffer, requiredAmountToFormat, activeCfgTemplate, mdl.guid, mdl.guid, mdl.guid, mdl.guid, mdl.guid, mdl.guid);

				// WriteFile(slnx, lineBuffer, requiredAmountToFormat - 1, &written, nullptr);

#ifdef MR_DEBUG
				// FlushFileBuffers(slnx);
#endif // MR_DEBUG
			}

			constexpr const char closeProjectConfigurationPlatforms[] = "\n\tEndGlobalSection\n";
			/// WriteFile(slnx, closeProjectConfigurationPlatforms, (sizeof(closeProjectConfigurationPlatforms) / sizeof(closeProjectConfigurationPlatforms[0])) - 1, &written, nullptr);

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
			//char* lineBuffer = (char*)outputFileBuffer.Allocate(requiredAmountToFormat);

			// snprintf(lineBuffer, requiredAmountToFormat, finalSolutionProperties, idStr);

			//WriteFile(slnx, lineBuffer, requiredAmountToFormat - 1, &written, nullptr);
			// CloseHandle(slnx);
		//}
		//else
		//{
		//	MR_LOG(LogFileConstruct, Error, "Failed to create finalization file for: %s", (*modules)[0].parent.Chr());
		//	MR_LOG(LogFileConstruct, Error, "CreateFileW failed with: %s", GetLastErrorString().Chr());
		//}

		////if (Commandline::Get().Check("-o"))
		//{
		//	MR_LOG(LogBuild, Log, "Opening trophy: %ls", fullPathToSln);
		//	ShellExecuteW(nullptr, L"open", fullPathToSln, nullptr, nullptr, SW_SHOWNORMAL);
		//}
	}

	void VcxprojGenerate(Module& mdl, const wchar_t* fullPathToIntermediateDir, u32& createdFilesCount)
	{
		//MR_LOG(LogFileConstruct, Log, "Creating %s project file", mdl.moduleName.Chr());

		//HANDLE proj = SendToOutputBuffer(fullPathToIntermediateDir, GENERIC_READ | GENERIC_WRITE, _placeholder_);
		//if (proj == INVALID_HANDLE_VALUE)
		//{
		//	MR_LOG(LogFileConstruct, Error, "Failed to create module file asset %s", mdl.moduleName.Chr());
		//	MR_LOG(LogFileConstruct, Error, "CreateFileW failed with: %s", GetLastErrorString().Chr());
		//	
		//	return;
		//};

		//MemoryBlockArena xmlAssembly = { 1_mB };
		//char* xmlBuffer = (char*)xmlAssembly.Exhaust();
		//u32 offset = 0;


		//auto Append = [&](const char* format, ...) 
		//{
		//	va_list args;
		//	va_start(args, format);

		//	u32 written = (u32)vsnprintf(xmlBuffer + offset, 1_mB - offset, format, args);

		//	if (written > 0) 
		//		offset += written;

		//	va_end(args);
		//};

		//Append(Project::Configurations);
		//Append(Project::ProjectIdentification, mdl.guid);

		//Append(Project::PropertyGroups);
		//Append(Project::Macros, mdl.moduleName.Chr());

		//char moduleNameUpper[64] = {};
		//for (u32 i = 0; i < mdl.moduleName.Length(); i++)
		//	moduleNameUpper[i] = toupper(mdl.moduleName.Chr()[i]);

		//MemoryBlockArena<char> includePathArena = { 256_kB };
		//char* includePath = (char*)includePathArena.Exhaust();
		//u32 loc = 0;

		//const Array<String> commands = mdl.commands["IncludePath"];
		//for (u32 i = 0; i < commands.GetSize(); i++)
		//{
		//	loc += snprintf(includePath + loc, commands[i].Length() + 3, "%s", commands[i].Chr());
		//	if (commands.GetSize() - 1 != i)
		//	{
		//		strcat(includePath + loc, ";");
		//		loc += 1;
		//	}
		//}

		//Append(Project::ConfigurationDependentSettings, mdl.moduleName.Chr(), moduleNameUpper, includePath, mdl.moduleName.Chr(), moduleNameUpper, includePath, mdl.moduleName.Chr(), moduleNameUpper, includePath);

		//MemoryBlockArena<char> filesArena = { 512_kB };
		//char* filesBuffer = (char*)filesArena.Exhaust();
		//u32 locFiles = 0;

		//for (const auto& file : mdl.files)
		//{
		//	wchar_t pathTo[MAX_PATH] = {};
		//	PathRelativePathToW(pathTo, fullPathToIntermediateDir, FILE_ATTRIBUTE_NORMAL, file, FILE_ATTRIBUTE_NORMAL);

		//	locFiles += snprintf(filesBuffer + locFiles, 512_kB - locFiles, Project::EntryFormat(Hash(PathFindExtensionW(pathTo))), pathTo);
		//}

		//Append(filesBuffer);

		//Append(Project::BottomPart);

		//DWORD written = 0;
		//WriteFile(proj, xmlBuffer, offset, &written, nullptr);

		//CloseHandle(proj);
		//createdFilesCount++;
	}
}
