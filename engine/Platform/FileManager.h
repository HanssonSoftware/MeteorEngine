/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Logging/Log.h>
#include <Platform/FileHelpers.h>
#undef CreateDirectory

class String;

LOG_ADDCATEGORY(FileManager);

struct IFileManager
{
	static bool CreateDirectory(const String* name, bool bForceCreate);

	static bool DeleteDirectory(const String& name, bool bToFullPath);
	
	static bool IsPathExists(const String& name);

	static bool IsPathRelative(const String& path);

	static bool IsEndingWith(const String& name, const String& extension);

	static void NormalizeDirectory(String& input);

	static IFile* CreateFileOperation(const String& path, int32_t accessType, int32_t sharingMode, int32_t createType);

private:
	IFileManager() = default;
};

#include "PlatformLayout.h"

#ifdef MR_PLATFORM_WINDOWS
#include <Windows/WindowsFileManager.h>
#endif // DEBUG
