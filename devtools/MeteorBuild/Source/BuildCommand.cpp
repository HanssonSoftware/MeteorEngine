/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Commands.h"
#include <Logging/Log.h>
#include <Commandlet.h>
#include <Memory/MemoryBlockArena.h>

#include "CommandRegistry.h"
#include "Module.h"

#include <Platform/Winapi.h>
#include <Shlwapi.h>

LOG_ADDCATEGORY(Commands);

namespace Commands
{
	ADD_NEW_BUILD_COMMAND("-build", "Builds your project", Build_Cmd);
	
	void Build_Cmd()
	{
		LARGE_INTEGER startTime, endTime, frequency;
		QueryPerformanceCounter(&startTime);

		const String sourceDirectory = Commandlet::Get().Parse("-s");
		const String intermediateDirectory = Commandlet::Get().Parse("-i");

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

			Array<wchar_t*> files;
			Array<FoundUnit> processedFiles;
			DirectorySearch(sourceDirectoryW, files, &arena);

			if (files.GetSize() < 0)
				return;

			for (auto& file : files)
			{
				wchar_t* extension = PathFindExtensionW(file);
				if (*extension == L'\0')
					continue;

				char extensionForHashing[16] = {};
				if (WideCharToMultiByte(CP_UTF8, 0, extension, wcslen(extension), extensionForHashing, 15, nullptr, nullptr) > 0)
				{
					switch (Hash(extensionForHashing))
					{
					case 3091167709698109830: // .cpp
					{
						processedFiles.Add({file, FoundUnit::Type::SOURCE});
						break;
					}
					case 9301237637030385942: // .natvis
					{
						processedFiles.Add({file, FoundUnit::Type::NATVIS});
						break;
					}
					case 7699041615178930058: // .mrbuild
					{
						processedFiles.Add({file, FoundUnit::Type::BUILD_SCRIPT});
						break;
					}
					default:
						break;
					}
				}
			}

			MemoryBlockArena<char> currentReadFile = { 8 * 1024 * 1024 };
			for (auto& processedFile : processedFiles)
			{
				if (processedFile.type != FoundUnit::Type::BUILD_SCRIPT)
					continue;

				HANDLE script = CreateFileW(processedFile.path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
				if (script != INVALID_HANDLE_VALUE)
				{
					LARGE_INTEGER size;
					GetFileSizeEx(script, &size);

					char* allocatedBufferForScript = (char*)currentReadFile.Allocate(size.QuadPart);

					DWORD actualRead = 0;
					if (ReadFile(script, allocatedBufferForScript, size.QuadPart, &actualRead, nullptr))
					{
						Module newModule = Module::MakeModuleFromBuffer(allocatedBufferForScript);
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
		}

		QueryPerformanceCounter(&endTime);
		QueryPerformanceFrequency(&frequency);

		MR_LOG(LogCommands, Log, "Build command ran in %.2f seconds!", (endTime.QuadPart - startTime.QuadPart) / (frequency.QuadPart / 100.0));
	}
}