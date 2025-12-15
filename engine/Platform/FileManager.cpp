/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#define _CRT_SECURE_NO_WARNINGS
#include "FileManager.h"
#include <Types/String.h>

#pragma warning(disable : 6031)

static_assert(!std::is_same_v<FileManager, IFileManager>, "File Manager is using BASE type!");

bool IFileManager::CreateDirectory(const String* name, bool bForceCreate)
{
	return false;
}

bool IFileManager::DeleteDirectory(const String& name, bool bToFullPath)
{
	return false;
}

bool IFileManager::IsPathExists(const String& name)
{
	return false;
}

bool IFileManager::IsPathRelative(const String& path)
{
	return false;
}

bool IFileManager::IsEndingWith(const String& name, const String& extension)
{
	return false;
}

void IFileManager::NormalizeDirectory(String& input)
{
	
}

IFile* IFileManager::CreateFileOperation(const String& path, int32_t accessType, int32_t sharingMode, int32_t createType)
{
	return nullptr;
}
