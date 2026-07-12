/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "ModuleManager.h"
#include <HAL/HAL.h>
#include <Application/Application.h>
#include <Special/EngineConstants.h>

#include "Win32/MinimalWin.h"

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
    for (Module*& module : modules)
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

typedef Module* (*fv)();

bool ModuleManager::LoadModule(const StringView& moduleName)
{
    if (IsModuleLoaded(moduleName))
        return true;

#ifdef MR_PLATFORM_WINDOWS
    wchar_t libraryName[512] = {};
    //const auto resultCount = swprintf(libraryName, 511, L"%s-%hs.dll", WIDE_ENGINE_NAME, moduleName.ptr);
    //MR_ASSERT(resultCount < 512, "Library formatting buffer is too small! (%d)", resultCount);
    HAL::ConvertToWide(libraryName, moduleName.size, (char*)moduleName.ptr);

    HMODULE module = LoadLibraryW(libraryName);
    if (module != nullptr)
    {
        fv moduleInstantiation = (fv)GetProcAddress(module, "InitialiseModule");

        if (moduleInstantiation)
        {
            Module* newModule = moduleInstantiation();
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
        Module*& module = modules[i];
        
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

bool ModuleManager::IsModuleLoaded(const StringView& moduleName)
{
    for (Module*& mdl : modules)
    {
        if (!mdl) continue;

        if (mdl->GetName().Chr() == (char*)moduleName.ptr)
        {
            return mdl->moduleState == Module::ModuleState::Enabled;
        }
    }

    return false;
}
