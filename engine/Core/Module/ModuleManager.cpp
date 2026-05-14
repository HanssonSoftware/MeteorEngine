/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "ModuleManager.h"
#include <Platform/Platform.h>
#include <Application/Application.h>
#include <Special/EngineConstants.h>

#include <Platform/Winapi.h>

#ifdef MR_PLATFORM_WINDOWS
#include <libloaderapi.h>
#endif // MR_PLATFORM_WINDOWS

#pragma warning(disable : 6387)

LOG_ADDCATEGORY(ModuleManager);

ModuleManager::ModuleManager() noexcept
{

}

ModuleManager& ModuleManager::Get()
{
    static ModuleManager instance;
    return instance;
}

ModuleManager::~ModuleManager() noexcept
{
    for (EngineModule*& module : modules)
    {
        module->ShutdownModule(); 

#ifdef MR_PLATFORM_WINDOWS   
        //if (!FreeLibrary(module->library))
        {
            MR_LOG(LogModuleManager, Error, "FreeLibrary returned: %ls (%ls)", L""/**Platform::GetError()*/, module->GetName().Chr());
            continue;
        }
#endif // MR_PLATFORM_WINDOWS
    }

    //modules.Reset();
}

typedef EngineModule* (*fv)();

bool ModuleManager::LoadModule(const String& moduleName)
{
    if (IsModuleLoaded(moduleName))
        return true;

#ifdef MR_PLATFORM_WINDOWS
    wchar_t libraryName[512] = {};
    const auto resultCount = swprintf(libraryName, 511, L"%s-%hs.dll", WIDE_ENGINE_NAME, *moduleName);

    MR_ASSERT(resultCount > 512, "Library formatting buffer is too small! (%d)", resultCount);

    HMODULE module = LoadLibraryW(libraryName);
    if (module != nullptr)
    {
        fv moduleInstantiation = (fv)GetProcAddress(module, "InitialiseModule");

        if (moduleInstantiation)
        {
            EngineModule* newModule = moduleInstantiation();
            newModule->name = moduleName;
            //newModule->library = module;
            newModule->StartupModule();

            modules.Add(newModule);

            return true;
        }
    }

#endif // MR_PLATFORM_WINDOWS


    return false;
}

bool ModuleManager::UnloadModule(const String& moduleName)
{
    const uint32_t moduleSize = modules.GetSize();
    for (uint32_t i = 0; i < moduleSize; i++)
    {
        EngineModule*& module = modules[i];
        
        if (module)
        {
            if (module->GetName() != moduleName)
                continue;

            module->ShutdownModule();

#ifdef MR_PLATFORM_WINDOWS
            //if (!FreeLibrary(module->library))
            {
                MR_LOG(LogModuleManager, Error, "FreeLibrary returned: %ls (%ls)", L"" /**Platform::GetError()*/, moduleName.Chr());
                return false;
            }
#else
            // FIXME: No non-windows implementation for module load!
#endif // MR_PLATFORM_WINDOWS

            //modules.Pop(i);
        }

        return true;
    }

    return true;
}

bool ModuleManager::IsModuleLoaded(const String& moduleName)
{
    for (EngineModule*& mdl : modules)
    {
        if (!mdl) continue;

        if (mdl->GetName() == moduleName)
        {
            return mdl->moduleState == ELoadState::ENABLED;
        }
    }

    return false;
}
