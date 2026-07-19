/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Commands.h"
#include <Logging/Log.h>
#include <HAL/Commandline.h>
#include <Memory/MemoryBlockArena.h>

#include "CommandRegistry.h"
#include "Module.h"

#include "Win32/MinimalWin.h"
#include <Shlwapi.h>
#include <PathCch.h>

LOG_ADDCATEGORY(Commands);
LOG_ADDCATEGORY(Build);
LOG_ADDCATEGORY(Validator);

namespace Commands
{
	ADD_NEW_BUILD_COMMAND("-build", "Builds your project", Build_Cmd);
	
	inline constexpr const u32 GetDebugLevel(const char* configuration)
	{
		switch (Hash(configuration))
		{
		case 3837802033854913258: // Development
			return 1;
		case 2626979548338364165: // Test
			return 2;
		case 16287715700692604995: // Shipping
			return 3;

		default:
			return 0;
		};
	};

	bool Build_Cmd()
	{
#ifdef MR_PLATFORM_WINDOWS
		PROCESS_INFORMATION pi = {};
		STARTUPINFOW si = { sizeof(STARTUPINFOW) };

		wchar_t clangCall[] = L"clang++";
		if (!CreateProcessW(nullptr, clangCall, nullptr, nullptr, 0, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi))
		{
			MR_LOG(LogValidator, Fatal, "Check clang++ is installed, or added as environment variable!");
			return false;
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
#endif // MR_PLATFORM_WINDOWS

		//const String sourceDirectory = Commandlet::Get().Parse("-s");
		//const String intermediateDirectory = Commandlet::Get().Parse("-i");
		//const String configuration = Commandlet::Get().Parse("-c");
		//const String productDirectory = Commandlet::Get().Parse("-p");

		// Arena allocates bytes, so equal size will be allocated!
		// WChar: 32 MB (Char / 2)
		// Char: 32 MB

#ifdef MR_PLATFORM_WINDOWS
		//static MemoryBlockArena<wchar_t> arena = { 64 * 1024 * 1024 };
#else
		//static MemoryBlockArena<char> arena = { 64 * 1024 * 1024 };
#endif // MR_PLATFORM_WINDOWS

		//if (sourceDirectory && intermediateDirectory)
		{
			//wchar_t* sourceDirectoryW = (wchar_t*)arena.Allocate(sourceDirectory.Length() * sizeof(wchar_t));
			//if (!MultiByteToWideChar(CP_UTF8, 0, sourceDirectory, sourceDirectory.Length(), sourceDirectoryW, sourceDirectory.Length()))
			{
				//MR_LOG(LogCommands, Error, "%s", *GetLastErrorString());
				return false;
			}

			// Collect files in folder

			Array<wchar_t*> files;
			//DirectorySearch(sourceDirectoryW, files, &arena);

			if (files.GetSize() < 0)
				return false;

			// Read scripts
			//MemoryBlockArena<char> currentReadFile = { 8 * 1024 * 1024 };

			Array<Module> modules;
			for (auto& file : files)
			{
				wchar_t* extension = PathFindExtensionW(file);
				if (*extension && wcscmp(extension, L".mrbuild") != 0)
					continue;

				//HANDLE script = SendToOutputBuffer(file, GENERIC_READ, _placeholder_);
				/*if (script != INVALID_HANDLE_VALUE)
				{
					LARGE_INTEGER size;
					GetFileSizeEx(script, &size);

					char* allocatedBufferForScript = (char*)currentReadFile.Allocate(size.QuadPart);

					DWORD actualRead = 0;
					if (ReadFile(script, allocatedBufferForScript, (u32)size.QuadPart, &actualRead, nullptr))
					{
						Module module = Module::MakeModuleFromBuffer(allocatedBufferForScript);

						u32 wideNameSize = wcslen(file);
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
				}*/
			}

			// Rank each module by significance

			Map<const char*, u32> ranking;
			for (Module& module : modules)
			{
				for (auto& a : module.commands["Dependencies"])
					ranking[a] += 1;
			}

			// Get thread count and start compiling

			//bool bIsMultiThreadEnabled = Commandlet::Get().Check("-mt");

#ifdef MR_PLATFORM_WINDOWS
			SYSTEM_INFO syi = {};
			GetSystemInfo(&syi);

			wchar_t exe[MAX_PATH] = {};
			wchar_t exea[512] = {};
			GetModuleFileNameW(nullptr, exe, MAX_PATH);


			//LogStandard(exea, L"%s\\..\\%hs\\%hs_Build.rsp", exe, intermediateDirectory.Chr(), Commandlet::Get().Parse("-m").Chr());
			//HANDLE responseFile = SendToOutputBuffer(exea, GENERIC_READ | GENERIC_WRITE, _placeholder_);
			//if (responseFile == INVALID_HANDLE_VALUE)
			{
				return false;
			}

			//MemoryBlockArena<char> buffer = { 1_mB };
			//char* responseBuffer = (char*)buffer.Exhaust();

			//const String currentModuleCompile = Commandlet::Get().Parse("-m");

			char moduleNameUpper[64] = {};
			//for (u32 i = 0; i < currentModuleCompile.Length(); i++)
				//moduleNameUpper[i] = toupper(currentModuleCompile.Chr()[i]);

			//const u32 debugLevel = GetDebugLevel(Commandlet::Get().Parse("-c").Chr());

			//const u32 count = (u32)sprintf(responseBuffer, "-std=c++17 -O%d %s %s -DMR_%s_EXPORT -shared -o %s.dll\n", debugLevel, debugLevel < 3 ? "-g" : "-flto", debugLevel < 3 ? "-DMR_DEBUG" : "", moduleNameUpper, currentModuleCompile.Chr());

			DWORD written = 0;
			//if (!WriteFile(responseFile, responseBuffer, count, &written, nullptr))
			{
				//MR_LOG(LogBuild, Error, "WriteFile failed with: %s", GetLastErrorString().Chr());
				return false;
			}

			//MemoryBlockArena<char> concatArena = { 512_kB };
			//char* concatBuffer = (char*)concatArena.Exhaust();

			u32 offset = 0;
			/*for (const wchar_t* file : files)
				offset += snprintf(concatBuffer + offset, 512_kB - offset, "%ls\n", file);*/

			//if (!WriteFile(responseFile, concatBuffer, offset, &written, nullptr))
			{
				//MR_LOG(LogBuild, Error, "WriteFile failed with: %s", GetLastErrorString().Chr());
				return false;
			}


			//if (bIsMultiThreadEnabled)
			{

			}
			//else
			{
				PROCESS_INFORMATION pi2 = {};
				STARTUPINFOW si2 = { sizeof(STARTUPINFOW) };
				si2.wShowWindow = SW_HIDE;

				wchar_t clangCall[256] = {};
				//swprintf(clangCall, L"clang++  ", exea, GetDebugLevel(configuration), L"d.dll", L"exports");

				if (!CreateProcessW(nullptr, clangCall, nullptr, nullptr, 0, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si2, &pi2))
				{
					MR_LOG(LogValidator, Fatal, "Error occoured while compiling source: %s", "VARIABLE NAME");
					return false;
				}

				WaitForSingleObject(pi2.hProcess, INFINITE);

				CloseHandle(pi2.hProcess);
				CloseHandle(pi2.hThread);

				//CloseHandle(responseFile);
				DeleteFileW(exea);
			}

#endif // MR_PLATFORM_WINDOWS
		}
	}
}