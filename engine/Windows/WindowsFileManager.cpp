/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#define _CRT_SECURE_NO_WARNINGS

#include "WindowsFileManager.h"
#include <Logging/Log.h>
#include <Application.h>
#include <Windows/WindowsPaths.h>
#include <Windows/WindowsFile.h>
#include <Platform/Platform.h>

#include <Windows.h>
#undef CreateDirectory

#include <shlwapi.h>
#include <PathCch.h>
#include <Shlobj.h>
#include <shellapi.h>

#pragma comment (lib, "Shlwapi.lib")
#pragma comment (lib, "Pathcch.lib")
#pragma comment (lib, "Shell32.lib")


bool WindowsFileManager::CreateDirectory(const String* name)
{
    if (name != nullptr)
    {
        //String newName = *name;
        //if (IsPathRelative(name))
        //{
        //    String dirAbsoluteConvert = Paths::GetExecutableDirctory();
        //    PathCchRemoveFileSpec(dirAbsoluteConvert.Data(), wcslen(dirAbsoluteConvert));

        //    wchar_t* combined = nullptr;
        //    if (FAILED(PathAllocCombine(dirAbsoluteConvert, newName, PATHCCH_ALLOW_LONG_PATHS, &combined)))
        //    {
        //        MR_LOG(LogFileManager, Error, "Failed to convert to full path: %ls", *Platform::GetError());
        //        return false;
        //    }

        //    newName = combined;
        //    LocalFree(combined);
        //}

        //for (wchar_t* p = newName.Data(); *p; ++p)
        //{
        //    if (*p == L'/')
        //        *p = L'\\';
        //}

        //const int32_t result = SHCreateDirectoryExW(nullptr, newName, nullptr);
        //if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS)
        //{
        //    MR_LOG(LogFileManager, Error, "SHCreateDirectory returned: %ls", *Platform::GetError());
        //    return false;
        //}

        return true;
    }

    MR_LOG(LogFileManager, Error, "Invalid directory name!");
    return false;
}

bool WindowsFileManager::DeleteDirectory(const String& name, bool bToFullPath)
{
    //const wchar_t* doubleTerminated = String::Format("%ls\0", name.Chr()).Chr();

    SHFILEOPSTRUCTW sh = {};
    sh.hwnd = nullptr;
    sh.wFunc = FO_DELETE;
    //sh.pFrom = doubleTerminated;
    sh.pTo = nullptr;
    sh.fFlags = FOF_MULTIDESTFILES | FOF_NO_UI;
    sh.fAnyOperationsAborted = false;
    sh.hNameMappings = nullptr;
    sh.lpszProgressTitle = nullptr;

    SHFileOperationW(&sh);

    if (sh.fAnyOperationsAborted != 0)
    {
        MR_LOG(LogFileManager, Error, "RemoveDirectoryW returned: %ls", *Platform::GetError());
        return false;
    }

    return true;
} 

bool WindowsFileManager::IsPathExists(const String* name)
{
    if (name != nullptr)
    {
        //if (!::PathFileExistsW(*name))
        {
            if (GetLastError() != ERROR_FILE_NOT_FOUND)
            {
                MR_LOG(LogFileManager, Error, "IsPathExists returned: %ls", *Platform::GetError());
            }

            return false;
        }

        return true;
    }

    return false;
}

/** Returns true if the path is qualified, or false otherwise. */
bool WindowsFileManager::IsPathRelative(const String* path)
{
    if (path != nullptr)
    {
        //return PathIsRelativeW(*path) ? true : false;
    }
    
    return false;
}

bool WindowsFileManager::IsEndingWith(const String& name, const String& extension)
{
    if (name.IsEmpty() && extension.IsEmpty())
        return false;

    //wchar_t* pointed = PathFindExtensionW(name.Chr());
    //pointed++; // (.)xy

    //String super(pointed, (uint32_t)wcslen(pointed));

    return "super == extension ? true : false";
}

void WindowsFileManager::NormalizeDirectory(String& input)
{
    //wchar_t* begin = input.Data();
    //for (wchar_t* p = begin; *p; p++)
    //{
    //    if (p == L"//")
    //    {

    //    }
    //    else if (*p == L'/')
    //    {

    //    }
    //}

    //input = begin;
}

IFile* WindowsFileManager::CreateFileOperation(String* pathToCreate, int32_t accessType, int32_t sharingMode, FileOverrideRules createType)
{
    if (pathToCreate != nullptr)
    {
        NormalizeDirectory(*pathToCreate);

        if (!IsPathExists(pathToCreate)) FileManager::CreateDirectory(pathToCreate);

        //HANDLE file = CreateFileW(
        //    *pathToCreate,
        //    evaluateAccessTypeForCreateFileOperation(accessType),
        //    evaluateSharingModeForCreateFileOperation(sharingMode),
        //    nullptr,
        //    evaluateCreateTypeForCreateFileOperation(createType),
        //    FILE_ATTRIBUTE_NORMAL,
        //    nullptr
        //);

        //if (file == INVALID_HANDLE_VALUE)
        {
            MR_LOG(LogFileManager, Error, "CreateFileW returned: %ls (%s)", *Platform::GetError(), **pathToCreate);
            return nullptr;
        }

        WindowsFile* newWindowsFile = new WindowsFile();
        //newWindowsFile->fileHandle = file;
        newWindowsFile->fileName = *pathToCreate;

        return newWindowsFile;
    }

    return nullptr;
}

constexpr const int32_t WindowsFileManager::evaluateAccessTypeForCreateFileOperation(int32_t accessType)
{
    int32_t accessTypeFast = 0;

    if (accessType & OPENMODE_NONE) accessTypeFast |= 0;
    if (accessType & OPENMODE_READ) accessTypeFast |= GENERIC_READ;
    if (accessType & OPENMODE_WRITE) accessTypeFast |= GENERIC_WRITE;

    return accessTypeFast;
}

constexpr const int32_t WindowsFileManager::evaluateSharingModeForCreateFileOperation(int32_t sharingMode)
{
    int32_t sharingModeFast = 0;

    if (sharingMode & SHAREMODE_NONE) sharingModeFast |= 0;
    if (sharingMode & SHAREMODE_READ) sharingModeFast |= FILE_SHARE_READ;
    if (sharingMode & SHAREMODE_WRITE) sharingModeFast |= FILE_SHARE_WRITE;
    if (sharingMode & SHAREMODE_DELETE) sharingModeFast |= FILE_SHARE_DELETE;

    return sharingModeFast;
}

constexpr const int32_t WindowsFileManager::evaluateCreateTypeForCreateFileOperation(FileOverrideRules createType)
{
    switch (createType)
    {
    case OVERRIDERULE_OPEN_ALWAYS:
        return 4;
    case OVERRIDERULE_OPEN_ONLY_IF_EXISTS:
        return 3;
    case OVERRIDERULE_CREATE_NEW_IF_NOT_EXISTS:
        return 1;
    case OVERRIDERULE_CREATE_NEW_DONT_MIND:
        return 2;
    default:
        return 0;
    }

    return 0;
}

// C:\\X\\Y\\FileName.txt
// ?\\FileName.txt

bool WindowsFileManager::startRecursiveCreate(wchar_t* dir)
{
    const uint32_t dirSize = (uint32_t)wcslen(dir);

    if (PathCchRemoveExtension(dir, dirSize) == S_OK)
        /*PathCchRemoveExtension*/ PathCchRemoveFileSpec(dir, dirSize);

    DWORD requiredAmount = GetCurrentDirectoryW(0, 0);
    wchar_t* super = new wchar_t[requiredAmount + 1]();
    GetCurrentDirectoryW(requiredAmount, super);

    wchar_t* superSuper;
    HRESULT result = PathAllocCombine(super, dir, PATHCCH_ALLOW_LONG_PATHS | PATHCCH_FORCE_ENABLE_LONG_NAME_PROCESS, &superSuper);

    int32_t resultA = SHCreateDirectory(0, superSuper);
    
    delete[] super;
    LocalFree(superSuper);
    
    return true;
}