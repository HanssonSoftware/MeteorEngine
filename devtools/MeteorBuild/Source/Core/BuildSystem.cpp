/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "BuildSystem.h"
#include <Types/StringW.h>
#include <Core/Log.h>

#include <Commandlet.h>
#include <Platform/FileManager.h>

#include "Utils.h"
#include <Platform/Paths.h>
#include <Platform/Platform.h>

#include <unordered_map>

#include <PathCch.h>
#include <Application.h>
#include "Parser.h"

#include <Shlobj.h>
#include "Application.h"
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Pathcch.lib")

#include <Methods/BuildProjectMethod.h>

LOG_ADDCATEGORY(BuildSystemFramework);
LOG_ADDCATEGORY(Assembler);

bool BuildSystem::InitFramework()
{
	BuildSystemLogger* bl = new BuildSystemLogger;
	*Logger::Get() = Logger(bl);

	bl->Initialize();
	hConsoleRef = bl->GetOutputHandle();

	static constexpr wchar_t header[] = L"========================== MeteorBuild (R) ==========================\n"
										L"=== Copyright 2020 - 2025, Hansson Software. All rights reserved. ===\r\n\0";

	DWORD written = 0;
	if (!WriteConsoleW(hConsoleRef, header, sizeof(header) / sizeof(header[0]), &written, nullptr))
	{
		MessageBoxW(GetConsoleWindow(), L"Failed to write header!", L"MeteorBuild(R) internal error!", MB_ICONERROR | MB_OK);
		return false;
	}

	if (Commandlet::Get().GetArgumentsCount() > 0)
	{
		if (!ReadArguments())
		{
			MessageBoxW(GetConsoleWindow(), L"No instruction parameter! Check help!", L"MeteorBuild(R) internal error!", MB_ICONERROR | MB_OK);
			return false;
		}
	}
	else
	{
		SendHelpInfo();
	}

	return true;
}

bool BuildSystem::ReadArguments()
{
	if (Commandlet::Get().Parse("-build", nullptr))
	{
		currentMethod = new BuildProjectMethod();
		return true;
	}
	else if (Commandlet::Get().Parse("-rebuild", nullptr))
	{
		//currentMethod = new RebuildProjectMethod;
	}
	else if (Commandlet::Get().Parse("-compile", nullptr))
	{

	}
	else if (Commandlet::Get().Parse("-clean", nullptr) || Commandlet::Get().Parse("-clear", nullptr))
	{

	}
	//else if (Commandlet::Get().Parse("-build", nullptr))
	//{

	//}
	//else if (Commandlet::Get().Parse("-build", nullptr))
	//{

	//}

	return false;
}

/** WARNING! This function uses, C++ standard. */
//void BuildSystem::OrderModules()
//{
	//std::unordered_map<std::string, uint32_t> ordering;
	//const uint32_t size = loadedModules.GetSize();

	//for (auto& module : loadedModules)
	//{
	//	for (auto& dependency : module.requires)
	//	{
	//		std::string depName = *dependency;
	//		ordering[depName]++;
	//	}
	//}

	//struct Pair
	//{
	//	uint32_t score;
	//	uint32_t index;
	//};

	//Array<Pair> scores;
	//scores.Resize(size);

	//for (uint32_t i = 0; i < size; ++i)
	//{
	//	auto& m = loadedModules[i];
	//	auto it = ordering.find(*m.moduleName);
	//	uint32_t score = (it != ordering.end()) ? it->second : 0;
	//	scores[i].score = score;
	//	scores[i].index = i;
	//}

	//for (uint32_t i = 0; i < size; ++i)
	//{
	//	for (uint32_t j = i + 1; j < size; ++j)
	//	{
	//		if (scores[j].score > scores[i].score)
	//		{
	//			auto tmp = scores[i];
	//			scores[i] = scores[j];
	//			scores[j] = tmp;
	//		}
	//	}
	//}

	//Array<Module> sorted;
	//sorted.Resize(size);

	//for (uint32_t k = 0; k < size; ++k)
	//{
	//	sorted[k] = loadedModules[scores[k].index];
	//}

	//loadedModules = std::move(sorted);

	//for (uint32_t i = 0; i < size; ++i)
	//{
	//	auto& m = loadedModules[i];
	//	auto it = ordering.find(*m.moduleName);
	//	uint32_t score = (it != ordering.end()) ? it->second : 0;
	//	MR_LOG(LogBuildSystemFramework, Log, "%ls (score %u)", *m.moduleName, score);
	//}
//}

void BuildSystem::SendHelpInfo() const
{
	const bool bIsHelpRequested = Commandlet::Get().Parse("-help", nullptr) || 
								  Commandlet::Get().Parse("-h", nullptr) || 
								  Commandlet::Get().Parse("?", nullptr);

	static constexpr const wchar_t helpA[] = L"\nDon\'t know what to do? List of options below.\n"
		L"Instruction parameters:\n"
		L"  -build\t\t-  Performs a build project method, requires a source, intermediate parameter!\n"
		L"  -rebuild\t\t-  Performs a full rebuild method, requires the same parameters as the -build, but it deletes everything.\n"
		L"\nOrdionary parameters:\n"
		L"  -help // -h // ?\t\t-  Brings up help (this pane).\n"
		L"  -intermediate // -int\t\t-  Sets the intermediate directory, for your generated project files.\n"
		L"  -source // -src // -s\t\t-  Sets the source directory, where your code lives and to search for .mrbuild scripts recursively.\n"
		L"  -solution // -sln\t\t-  Sets an alternative solution directory, when selected instruction is done your solution will be copied into that path.\n";

	static constexpr const wchar_t helpB[] = L"\nNo parameters in input. Don\'t know what to do? Little help.\n"
		L"Instruction parameters:\n"
		L"  -build\t\t-  Performs a build project method, requires a source, intermediate parameter.\n"
		L"  -rebuild\t\t-  Performs a full rebuild method, requires the same parameters as the -build, but it deletes everything.\n"
		L"\nOrdionary parameters:\n"
		L"  -help // -h // ?\t\t-  Brings up help (this pane).\n"
		L"  -intermediate // -int\t\t-  Sets the intermediate directory, for your generated project files.\n"
		L"  -source // -src // -s\t\t-  Sets the source directory, where your code lives and to search for .mrbuild scripts recursively.\n"
		L"  -solution // -sln\t\t-  Sets an alternative solution directory, when selected instruction is done your solution will be copied into that path.\n";

	static constexpr const uint32_t helpASize = sizeof(helpA) / sizeof(helpA[0]);
	static constexpr const uint32_t helpBSize = sizeof(helpB) / sizeof(helpB[0]);

	DWORD written = 0;
	if (!WriteConsoleW(hConsoleRef, bIsHelpRequested ? helpA : helpB, bIsHelpRequested ? helpASize : helpBSize, &written, nullptr))
	{
		MessageBoxW(GetConsoleWindow(), L"Failed to write help!", L"MeteorBuild(R) internal error!", MB_ICONERROR | MB_OK);
		return;
	}
}

bool BuildSystem::BuildProjectFiles()
{
	//String intermediateLocation;
	//if (loadedModules.GetSize() > 0 && Commandlet::Get().Parse("-int", &intermediateLocation))
	//{
	//	if (FileManager::IsPathRelative(&intermediateLocation))
	//	{
	//		wchar_t exePathRaw[MAX_PATH] = { L'\0' };
	//		GetModuleFileNameW(GetModuleHandle(nullptr), exePathRaw, MAX_PATH);

	//		PathCchRemoveFileSpec(exePathRaw, wcslen(exePathRaw));

	//		PWSTR combinedPathNonCanonicalized;
	//		PathAllocCombine(exePathRaw, intermediateLocation, PATHCCH_ALLOW_LONG_PATHS, &combinedPathNonCanonicalized);

	//		intermediateLocation = combinedPathNonCanonicalized;
	//		LocalFree(combinedPathNonCanonicalized);
	//	}

	//	intermediateLocation = String::Format("%ls\\%ls", *intermediateLocation, *GetApplication()->GetApplicationCodeName()); // C:\\Meteor-Engine\\Intermediate\\Apollo
	//	if (SHCreateDirectoryExW(nullptr, intermediateLocation, nullptr) == ERROR_ALREADY_EXISTS)
	//	{
	//		SHFILEOPSTRUCTW sh = {};
	//		sh.hwnd = nullptr;
	//		sh.wFunc = FO_DELETE;
	//		//sh.pFrom = doubleTerminated;
	//		sh.pTo = nullptr;
	//		sh.fFlags = FOF_MULTIDESTFILES | FOF_NO_UI;
	//		sh.fAnyOperationsAborted = false;
	//		sh.hNameMappings = nullptr;
	//		sh.lpszProgressTitle = nullptr;

	//		SHFileOperationW(&sh);

	//		if (sh.fAnyOperationsAborted != 0)
	//		{
	//			MR_LOG(LogFileManager, Error, "RemoveDirectoryW returned: %ls", *Platform::GetError());
	//			return false;
	//		}

	//		FileManager::CreateDirectory(&intermediateLocation);
	//	}

	//	String generated = String::Format("%ls\\%lsDefs.h", *intermediateLocation, *GetApplication()->GetApplicationCodeName());
	//	GenerateImportExportDefinitions(&generated);

	//	for (auto& module : loadedModules)
	//	{
	//		const String directoryToCreateTheFolder = String::Format("%ls\\%ls", *intermediateLocation, *module.moduleName);
	//		FileManager::CreateDirectory(&directoryToCreateTheFolder);

	//		module.generatedProjectFile = String::Format("%ls\\%ls.vcxproj", *directoryToCreateTheFolder, *module.moduleName);

	//		HANDLE actual = CreateFileW(module.generatedProjectFile, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	//		if (actual != INVALID_HANDLE_VALUE)
	//		{
	//			String final;
	//			if (module.ConstructProjectFile(&final))
	//			{
	//				ScopedPtr<char> skinnyBuffer = Platform::ConvertToNarrow(final);

	//				DWORD written = 0;
	//				if (!WriteFile(actual, skinnyBuffer.Get(), final.Length(), &written, nullptr))
	//				{
	//					MR_LOG(LogAssembler, Error, "WriteFile returned: %ls", *Platform::GetError());
	//				}
	//			}

	//			CloseHandle(actual);
	//		}
	//		else
	//		{
	//			MR_LOG(LogAssembler, Fatal, "Failed to create project file at: %ls", *module.generatedProjectFile);
	//		}

	//		HANDLE header = CreateFileW(String::Format("%ls\\%ls.proxy.h", *directoryToCreateTheFolder, *module.moduleName), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	//		if (header != INVALID_HANDLE_VALUE)
	//		{
	//			String final;
	//			if (module.GenerateProxyHeader(&final))
	//			{
	//				ScopedPtr<char> skinnyBuffer = Platform::ConvertToNarrow(final);

	//				DWORD written = 0;
	//				if (!WriteFile(header, skinnyBuffer.Get(), final.Length(), &written, nullptr))
	//				{
	//					MR_LOG(LogAssembler, Error, "WriteFile returned: %ls", *Platform::GetError());
	//				}
	//			}

	//			CloseHandle(header);
	//		}
	//		else
	//		{
	//			MR_LOG(LogAssembler, Fatal, "Failed to create proxy header file at: %ls", *directoryToCreateTheFolder);
	//		}

	//		HANDLE bootstrap = CreateFileW(String::Format("%ls\\%ls.bootstrap.cpp", *directoryToCreateTheFolder, *module.moduleName), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	//		if (bootstrap != INVALID_HANDLE_VALUE)
	//		{
	//			String final;
	//			if (module.GenerateBootstrapHeader(&final))
	//			{
	//				ScopedPtr<char> skinnyBuffer = Platform::ConvertToNarrow(final);

	//				DWORD written = 0;
	//				if (!WriteFile(bootstrap, skinnyBuffer.Get(), final.Length(), &written, nullptr))
	//				{
	//					MR_LOG(LogAssembler, Error, "WriteFile returned: %ls", *Platform::GetError());
	//				}
	//			}

	//			CloseHandle(bootstrap);
	//		}
	//		else
	//		{
	//			MR_LOG(LogAssembler, Fatal, "Failed to create bootstrap header file at: %ls", *directoryToCreateTheFolder);
	//		}


	//	}

	//	String sourceDir;
	//	if (Commandlet::Get().Parse("-source", &sourceDir))
	//	{
	//		String exeDir = WindowsPaths::GetExecutableDirctory();
	//		PathCchRemoveFileSpec(exeDir.Data(), exeDir.Length());

	//		PWSTR combinedPathNonCanonicalized;
	//		PathAllocCombine(exeDir, sourceDir, PATHCCH_ALLOW_LONG_PATHS, &combinedPathNonCanonicalized);

	//		PathCchRemoveFileSpec(combinedPathNonCanonicalized, wcslen(combinedPathNonCanonicalized));

	//		sourceDir = combinedPathNonCanonicalized;
	//		LocalFree(combinedPathNonCanonicalized);

	//		HANDLE solution = CreateFileW(String::Format("%ls\\%ls.slnx", *sourceDir, *ps->projectName), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	//		if (solution != INVALID_HANDLE_VALUE)
	//		{
	//			String buffer;
	//			if (ps->Finalize(&buffer))
	//			{
	//				ScopedPtr<char> skinnyBuffer = Platform::ConvertToNarrow(buffer);

	//				DWORD written = 0;
	//				if (!WriteFile(solution, skinnyBuffer.Get(), buffer.Length(), &written, nullptr))
	//				{
	//					MR_LOG(LogAssembler, Error, "WriteFile returned: %ls", *Platform::GetError());
	//				}
	//			}

	//			CloseHandle(solution);
	//			return true;
	//		}
	//	}
	//}

	return false;
}

//bool BuildSystem::GenerateImportExportDefinitions(String* path)
//{
//	//HANDLE importExports = CreateFileW(*path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
//	//if (importExports != INVALID_HANDLE_VALUE)
//	//{
//	//	static constexpr const wchar_t buffer[] =
//	//		L"/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */\n\n/* Automatically generated by MeteorBuild(R) */\n"
//	//		L"\n#pragma once\n"
//	//		L"#define DLLEXPORT __declspec(dllexport)\n"
//	//		L"#define DLLIMPORT __declspec(dllimport)\0"
//	//	;
//	//	
//	//	ScopedPtr<char> skinnyBuffer = Platform::ConvertToNarrow(buffer);
//
//	//	DWORD written = 0;
//	//	if (!WriteFile(importExports, skinnyBuffer.Get(), sizeof(buffer) / sizeof(wchar_t), &written, nullptr))
//	//	{
//	//		MR_LOG(LogAssembler, Error, "WriteFile returned: %ls", *Platform::GetError());
//	//	}
//	//	
//
//	//	CloseHandle(importExports);
//	//	return true;
//	//}
//
//	return false;
//}

//Module* BuildSystem::FindModule(const String* name)
//{
//	//static Module* last = nullptr;
//	//static String lastName;
//	//if (lastName == *name)
//	//	return last;
//
//	//for (auto& mdl : loadedModules)
//	//{
//	//	//if (mdl.moduleName == *name)
//	//	//{
//	//	//	lastName = *name;
//	//	//	last = &mdl;
//
//	//	//	return &mdl;
//	//	//}
//	//}
//
//	return nullptr;
//}
