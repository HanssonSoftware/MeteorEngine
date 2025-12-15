/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/FileManager.h>
#undef CreateDirectory

#include <Types/String.h>

class IFile;

struct WindowsFileManager : public IFileManager
{
	static bool CreateDirectory(const String* name);

	static bool DeleteDirectory(const String& name, bool bToFullPath);

	static bool IsPathExists(const String* name);

	/** Returns true if the path is qualified, or false otherwise. */
	static bool IsPathRelative(const String* path);

	static bool IsEndingWith(const String& name, const String& extension);

	static void NormalizeDirectory(String& input);

	static IFile* CreateFileOperation(String* pathToCreate, int32_t accessType, int32_t sharingMode, FileOverrideRules createType);
private:
	WindowsFileManager() = default;

	static constexpr const int32_t evaluateAccessTypeForCreateFileOperation(int32_t accessType);

	static constexpr const int32_t evaluateSharingModeForCreateFileOperation(int32_t sharingMode);

	static constexpr const int32_t evaluateCreateTypeForCreateFileOperation(FileOverrideRules createType);

	static bool startRecursiveCreate(wchar_t* dir);
};

using FileManager = WindowsFileManager;