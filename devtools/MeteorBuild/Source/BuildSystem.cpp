/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "BuildSystem.h"
#include <Types/String.h>

#include <Commandlet.h>

#include <Platform/Platform.h>

#include <unordered_map>

#include <PathCch.h>
#include "Application.h"
#include "Parser.h"

#include <Shlobj.h>
#include "Application.h"
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Pathcch.lib")

#include <BuildProjectMethod.h>

LOG_ADDCATEGORY(BuildSystemFramework);
LOG_ADDCATEGORY(Assembler);

bool BuildSystem::UseBuildSystem()
{
	hConsoleRef = GetStdHandle(STD_OUTPUT_HANDLE);

	constexpr wchar_t header[] = L"========================== MeteorBuild (R) ==========================\n"
								 L"=== Copyright 2020 - 2026, Hansson Software. All rights reserved. ===\n";

	DWORD written = 0;
	if (!WriteConsoleW(hConsoleRef, header, sizeof(header) / sizeof(header[0]), &written, nullptr))
	{
		MessageBoxW(GetConsoleWindow(), L"Failed to write header!", L"MeteorBuild(R) internal error!", MB_ICONERROR | MB_OK);
		return false;
	}

	if (!ReadArguments())
		SendHelpInfo();

	currentMethod->StartMethod();
	return true;
}

void BuildSystem::Shutdown()
{
	if (currentMethod) delete[] currentMethod;
}

bool BuildSystem::ReadArguments()
{
	currentMethod = Commandlet::Get().Check("-build") ? new BuildProjectMethod() :
		Commandlet::Get().Check("-rebuild") ? new BuildProjectMethod() :
		Commandlet::Get().Check("-compile") ? new BuildProjectMethod() :
		Commandlet::Get().Check("-clean") ? new BuildProjectMethod() : nullptr;

	return currentMethod ? true : false;
}

void BuildSystem::SendHelpInfo() const
{
	const bool bIsHelpRequested = Commandlet::Get().Check("-help") ||
								  Commandlet::Get().Check("-h") ||
								  Commandlet::Get().Check("?");

	static constexpr const char helpBuffer[] = "\nNo parameters in input. Don\'t know what to do? Little help.\n"
		"Instruction parameters:\n"
		"  -build\t\t-  Performs a build project method, requires a source, intermediate parameter.\n"
		"  -rebuild\t\t-  Performs a full rebuild method, requires the same parameters as the -build, but it deletes everything.\n"
		"\nOrdionary parameters:\n"
		"  -help // -h // ?\t\t-  Brings up help (this pane).\n"
		"  -intermediate // -int\t\t-  Sets the intermediate directory, for your generated project files.\n"
		"  -source // -src // -s\t\t-  Sets the source directory, where your code lives and to search for .mrbuild scripts recursively.\n"
		"  -solution // -sln\t\t-  Sets an alternative solution directory, when selected instruction is done your solution will be copied into that path.\n";

	static constexpr const u32 helpSize = sizeof(helpBuffer) / sizeof(helpBuffer[0]);

#ifdef MR_PLATFORM_WINDOWS

	wchar_t fixedBufferForConverting[helpSize] = {};
	if (MultiByteToWideChar(CP_UTF8, 0, helpBuffer, helpSize, fixedBufferForConverting, helpSize))
	{
		DWORD written = 0;
		if (!WriteConsoleW(hConsoleRef, fixedBufferForConverting, helpSize, &written, nullptr))
		{
			MessageBoxW(GetConsoleWindow(), L"Failed to write help!", L"MeteorBuild(R) internal error!", MB_ICONERROR | MB_OK);
			return;
		}
	}
	else
	{
		MessageBoxW(GetConsoleWindow(), L"MultiByteToWideChar", L"MeteorBuild(R) internal error!", MB_ICONERROR | MB_OK);
		return;
	}
#else 
#endif // MR_PLATFORM_WINDOWS
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
//	//		L"/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */\n\n/* Automatically generated by MeteorBuild(R) */\n"
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
