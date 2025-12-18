/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "BuildSystem.h"

#include <Commandlet.h>
#include <Platform/FileManager.h>
#include <Module/Project.h>

#include "Utils.h"
#include <Platform/Paths.h>
#include <Platform/Platform.h>

#include <unordered_map>

#include <PathCch.h>
#include <Application.h>
#include "Parser.h"

#include "Windows/WindowsPaths.h"

LOG_ADDCATEGORY(BuildSystemFramework);
LOG_ADDCATEGORY(Assembler);

bool BuildSystem::InitFramework()
{
	if (ReadArguments())
	{
		bool bHasProject = false, bAtLeastOneScriptParsed = false;
		String sourceDirectoryFromLaunchParameter;

		if (Commandlet::Parse("-source", &sourceDirectoryFromLaunchParameter))
		{
			Array<String> filesFoundInSources;
			Array<String> scriptsFound;

			Utils::ListDirectory(&sourceDirectoryFromLaunchParameter, filesFoundInSources);
			for (auto& pathToDiscoveredItemsIndexed : filesFoundInSources)
			{
				//const String combined = String::Format("%ls\\%ls", *pathToDiscoveredItemsIndexed.path, *pathToDiscoveredItemsIndexed.name);
				if (FileManager::IsEndingWith(pathToDiscoveredItemsIndexed, "mrbuild"))
				{
					scriptsFound.Add(pathToDiscoveredItemsIndexed);
					MR_LOG(LogBuildSystemFramework, Verbose, "Found script: %ls", *pathToDiscoveredItemsIndexed);
				}
			}

			const uint32_t max = scriptsFound.GetSize() /* Be aware! The last one is always should be the project script!*/;
			for (uint32_t i = 0; i < max; i++)
			{
				String indexed = scriptsFound[i];

				Module* mdl = Parser::ParseModuleScript(&indexed);
				if (mdl != nullptr)
				{
					if (!bAtLeastOneScriptParsed) bAtLeastOneScriptParsed = true;

					Array<String> sd;
					Utils::ListDirectory(&indexed, sd);

					for (auto& temp : sd)
					{
						mdl->files.Add(*temp);
						MR_LOG(LogBuildSystemFramework, Verbose, "%ls module, new file added to include list: %ls", *mdl->moduleName, *temp);
					}

					loadedModules.Add(*mdl);
				}
				else
				{
					ps = Parser::ParseProjectScript(&indexed);
					if (ps != nullptr)
					{
						bHasProject = true;
					}
				}
			}
		}

		return bHasProject && bAtLeastOneScriptParsed;
	}


	return false;
}

/** WARNING! This function uses, C++ standard. */
void BuildSystem::OrderModules()
{
	std::unordered_map<std::wstring, uint32_t> ordering;
	const uint32_t size = loadedModules.GetSize();

	for (auto& module : loadedModules)
	{
		for (auto& dependency : module.requires)
		{
			std::wstring depName = *dependency;
			ordering[depName]++;
		}
	}

	struct Pair
	{
		uint32_t score;
		uint32_t index;
	};

	Array<Pair> scores;
	scores.Resize(size);

	for (uint32_t i = 0; i < size; ++i)
	{
		auto& m = loadedModules[i];
		auto it = ordering.find(*m.moduleName);
		uint32_t score = (it != ordering.end()) ? it->second : 0;
		scores[i].score = score;
		scores[i].index = i;
	}

	for (uint32_t i = 0; i < size; ++i)
	{
		for (uint32_t j = i + 1; j < size; ++j)
		{
			if (scores[j].score > scores[i].score)
			{
				auto tmp = scores[i];
				scores[i] = scores[j];
				scores[j] = tmp;
			}
		}
	}

	Array<Module> sorted;
	sorted.Resize(size);

	for (uint32_t k = 0; k < size; ++k)
	{
		sorted[k] = loadedModules[scores[k].index];
	}

	loadedModules = std::move(sorted);

	for (uint32_t i = 0; i < size; ++i)
	{
		auto& m = loadedModules[i];
		auto it = ordering.find(*m.moduleName);
		uint32_t score = (it != ordering.end()) ? it->second : 0;
		MR_LOG(LogBuildSystemFramework, Log, "%ls (score %u)", *m.moduleName, score);
	}
}

bool BuildSystem::BuildProjectFiles()
{
	String intermediateLocation;
	if (loadedModules.GetSize() > 0 && Commandlet::Parse("-int", &intermediateLocation))
	{
		if (FileManager::IsPathRelative(&intermediateLocation))
		{
			String exeDir = Paths::GetExecutableDirctory();

			PathCchRemoveFileSpec(exeDir.Data(), exeDir.Length());
			exeDir.Refresh();

			PWSTR combinedPathNonCanonicalized;
			PathAllocCombine(exeDir, intermediateLocation, PATHCCH_ALLOW_LONG_PATHS, &combinedPathNonCanonicalized);

			intermediateLocation = combinedPathNonCanonicalized;
			LocalFree(combinedPathNonCanonicalized);
		}

		intermediateLocation = String::Format("%ls\\%ls", *intermediateLocation, *GetApplication()->GetApplicationCodeName()); // C:\\Meteor-Engine\\Intermediate\\Apollo
		FileManager::CreateDirectory(&intermediateLocation);
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			FileManager::DeleteDirectory(intermediateLocation, true);
			FileManager::CreateDirectory(&intermediateLocation);
		}

		String generated = String::Format("%ls\\%lsDefs.h", *intermediateLocation, *GetApplication()->GetApplicationCodeName());
		GenerateImportExportDefinitions(&generated);

		for (auto& module : loadedModules)
		{
			const String directoryToCreateTheFolder = String::Format("%ls\\%ls", *intermediateLocation, *module.moduleName);
			FileManager::CreateDirectory(&directoryToCreateTheFolder);

			module.generatedProjectFile = String::Format("%ls\\%ls.vcxproj", *directoryToCreateTheFolder, *module.moduleName);

			HANDLE actual = CreateFileW(module.generatedProjectFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (actual != INVALID_HANDLE_VALUE)
			{
				String final;
				if (module.ConstructProjectFile(&final))
				{
					ScopedPtr<char> skinnyBuffer = Platform::ConvertToNarrow(final);

					DWORD written = 0;
					if (!WriteFile(actual, skinnyBuffer.Get(), final.Length(), &written, nullptr))
					{
						MR_LOG(LogAssembler, Error, "WriteFile returned: %ls", *Platform::GetError());
					}
				}

				CloseHandle(actual);
			}
			else
			{
				MR_LOG(LogAssembler, Fatal, "Failed to create project file at: %ls", *module.generatedProjectFile);
			}

			HANDLE header = CreateFileW(String::Format("%ls\\%ls.proxy.h", *directoryToCreateTheFolder, *module.moduleName), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (header != INVALID_HANDLE_VALUE)
			{
				String final;
				if (module.GenerateProxyHeader(&final))
				{
					ScopedPtr<char> skinnyBuffer = Platform::ConvertToNarrow(final);

					DWORD written = 0;
					if (!WriteFile(header, skinnyBuffer.Get(), final.Length(), &written, nullptr))
					{
						MR_LOG(LogAssembler, Error, "WriteFile returned: %ls", *Platform::GetError());
					}
				}

				CloseHandle(header);
			}
			else
			{
				MR_LOG(LogAssembler, Fatal, "Failed to create proxy header file at: %ls", *directoryToCreateTheFolder);
			}

			HANDLE bootstrap = CreateFileW(String::Format("%ls\\%ls.bootstrap.cpp", *directoryToCreateTheFolder, *module.moduleName), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (bootstrap != INVALID_HANDLE_VALUE)
			{
				String final;
				if (module.GenerateBootstrapHeader(&final))
				{
					ScopedPtr<char> skinnyBuffer = Platform::ConvertToNarrow(final);

					DWORD written = 0;
					if (!WriteFile(bootstrap, skinnyBuffer.Get(), final.Length(), &written, nullptr))
					{
						MR_LOG(LogAssembler, Error, "WriteFile returned: %ls", *Platform::GetError());
					}
				}

				CloseHandle(bootstrap);
			}
			else
			{
				MR_LOG(LogAssembler, Fatal, "Failed to create bootstrap header file at: %ls", *directoryToCreateTheFolder);
			}


		}

		String sourceDir;
		if (Commandlet::Parse("-source", &sourceDir))
		{
			String exeDir = WindowsPaths::GetExecutableDirctory();
			PathCchRemoveFileSpec(exeDir.Data(), exeDir.Length());

			PWSTR combinedPathNonCanonicalized;
			PathAllocCombine(exeDir, sourceDir, PATHCCH_ALLOW_LONG_PATHS, &combinedPathNonCanonicalized);

			PathCchRemoveFileSpec(combinedPathNonCanonicalized, wcslen(combinedPathNonCanonicalized));

			sourceDir = combinedPathNonCanonicalized;
			LocalFree(combinedPathNonCanonicalized);

			HANDLE solution = CreateFileW(String::Format("%ls\\%ls.slnx", *sourceDir, *ps->projectName), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (solution != INVALID_HANDLE_VALUE)
			{
				String buffer;
				if (ps->Finalize(&buffer))
				{
					ScopedPtr<char> skinnyBuffer = Platform::ConvertToNarrow(buffer);

					DWORD written = 0;
					if (!WriteFile(solution, skinnyBuffer.Get(), buffer.Length(), &written, nullptr))
					{
						MR_LOG(LogAssembler, Error, "WriteFile returned: %ls", *Platform::GetError());
					}
				}

				CloseHandle(solution);
				return true;
			}
		}
	}

	return false;
}

bool BuildSystem::GenerateImportExportDefinitions(String* path)
{
	HANDLE importExports = CreateFileW(*path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (importExports != INVALID_HANDLE_VALUE)
	{
		static constexpr const wchar_t buffer[] =
			L"/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */\n\n/* Automatically generated by MeteorBuild(R) */\n"
			L"\n#pragma once\n"
			L"#define DLLEXPORT __declspec(dllexport)\n"
			L"#define DLLIMPORT __declspec(dllimport)\0"
		;
		
		ScopedPtr<char> skinnyBuffer = Platform::ConvertToNarrow(buffer);

		DWORD written = 0;
		if (!WriteFile(importExports, skinnyBuffer.Get(), sizeof(buffer) / sizeof(wchar_t), &written, nullptr))
		{
			MR_LOG(LogAssembler, Error, "WriteFile returned: %ls", *Platform::GetError());
		}
		

		CloseHandle(importExports);
		return true;
	}

	return false;
}

Module* BuildSystem::FindModule(const String* name)
{
	static Module* last = nullptr;
	static String lastName;
	if (lastName == *name)
		return last;

	for (auto& mdl : loadedModules)
	{
		if (mdl.moduleName == *name)
		{
			lastName = *name;
			last = &mdl;

			return &mdl;
		}
	}

	return nullptr;
}

bool BuildSystem::ReadArguments()
{
	if (Commandlet::Parse("-build", nullptr))
	{
		command = Build;
		return true;
	}
	else if (Commandlet::Parse("-rebuild", nullptr))
	{
		command = Rebuild;
		return true;
	}
	else
	{
		MR_LOG(LogBuildSystemFramework, Fatal, "Main parameter is missing! Use -build or -rebuild");
	}

	return false;
}

