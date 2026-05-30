/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Commands.h"
#include <Logging/Log.h>
#include <Commandlet.h>
#include <Memory/MemoryBlockArena.h>

#include "CommandRegistry.h"
#include "Module.h"

#include <Platform/Winapi.h>
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

	void MakeModuleToForwardableCommand(Module* module, Array<wchar_t*>& commands)
	{

	}

	void Build_Cmd()
	{
#ifdef MR_PLATFORM_WINDOWS
		PROCESS_INFORMATION pi = {};
		STARTUPINFOW si = { sizeof(STARTUPINFOW) };

		wchar_t clangCall[] = L"clang++";
		if (!CreateProcessW(nullptr, clangCall, nullptr, nullptr, 0, NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, nullptr, nullptr, &si, &pi))
		{
			MR_LOG(LogValidator, Fatal, "Check clang++ is installed, or added as environment variable!");
			return;
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
#endif // MR_PLATFORM_WINDOWS

		LARGE_INTEGER startTime, endTime, frequency;
		QueryPerformanceCounter(&startTime);

		const String sourceDirectory = Commandlet::Get().Parse("-s");
		const String intermediateDirectory = Commandlet::Get().Parse("-i");
		const String configuration = Commandlet::Get().Parse("-c");
		const String productDirectory = Commandlet::Get().Parse("-p");

		// Arena allocates bytes, so equal size will be allocated!
		// WChar: 32 MB (Char / 2)
		// Char: 32 MB

#ifdef MR_PLATFORM_WINDOWS
		static MemoryBlockArena<wchar_t> arena = { 64 * 1024 * 1024 };
#else
		static MemoryBlockArena<char> arena = { 64 * 1024 * 1024 };
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
					if (ReadFile(script, allocatedBufferForScript, size.QuadPart, &actualRead, nullptr))
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
				}
			}

			// Rank each module by significance

			Map<const char*, u32> ranking;
			for (Module& module : modules)
			{
				for (auto& a : module.commands["Dependencies"])
					ranking[a] += 1;
			}

			// Get thread count and start compiling

			bool bIsMultiThreadEnabled = Commandlet::Get().Check("-mt");

#ifdef MR_PLATFORM_WINDOWS
			SYSTEM_INFO syi = {};
			GetSystemInfo(&syi);

			Array<wchar_t*> forwardingCommandsToCompiler;

			if (bIsMultiThreadEnabled)
			{

			}
			else
			{
				


				PROCESS_INFORMATION pi2 = {};
				STARTUPINFOW si2 = { sizeof(STARTUPINFOW) };
				si2.wShowWindow = SW_HIDE;

				wchar_t* sources;

				wchar_t clangCall[4096] = {};
				//swprintf(clangCall, L"clang++ %s -O%d -o %s -D%s -shared", ,GetDebugLevel(configuration), L"d.obj", L"exports");

				if (!CreateProcessW(nullptr, clangCall, nullptr, nullptr, 0, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si2, &pi2))
				{
					MR_LOG(LogValidator, Fatal, "Error occoured while compiling source: %s", "VARIABLE NAME");
					return;
				}

				WaitForSingleObject(pi2.hProcess, INFINITE);

				CloseHandle(pi2.hProcess);
				CloseHandle(pi2.hThread);
			}

#endif // MR_PLATFORM_WINDOWS
		}

		QueryPerformanceCounter(&endTime);
		QueryPerformanceFrequency(&frequency);

		MR_LOG(LogCommands, Log, "Build command ran in %.4f seconds!", (endTime.QuadPart - startTime.QuadPart) / frequency.QuadPart);
	}
}