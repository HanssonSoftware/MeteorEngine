/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "ModuleManager.h"
#include <Platform/PlatformLayout.h>
#include <Platform/Platform.h>
#include <Application.h>

#ifdef MR_PLATFORM_WINDOWS
#include <Windows/Windows.h>
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
        if (!FreeLibrary(module->library))
        {
            MR_LOG(LogModuleManager, Error, "FreeLibrary returned: %ls (%ls)", L""/**Platform::GetError()*/, module->GetName().Chr());
            continue;
        }
#endif // MR_PLATFORM_WINDOWS
    }

    modules.Reset();
}

static constexpr const char* defaultEngineName = "MeteorEngine";
typedef Module* (*fv)();

bool ModuleManager::LoadModule(const String& moduleName)
{
    if (IsModuleLoaded(moduleName))
        return true;

#ifdef MR_PLATFORM_WINDOWS
    String libraryName = String::Format("%ls-%ls.dll", *GetApplication()->GetApplicationName(), moduleName.Chr());

    HMODULE module = LoadLibraryW(/*libraryName*/L"");
    if (module != nullptr)
    {
        fv moduleInstantiation = (fv)GetProcAddress(module, "InitialiseModule");

        if (moduleInstantiation)
        {
            Module* newModule = moduleInstantiation();
            newModule->name = moduleName;
            newModule->library = module;
            newModule->StartupModule();

            modules.Add(newModule);

            return true;
        }
    }
    else
    {
        libraryName = String::Format("%ls-%ls.dll", defaultEngineName, moduleName.Chr());

        module = LoadLibraryW(/*libraryName*/L"");
        if (module != nullptr)
        {
            fv moduleInstantiation = (fv)GetProcAddress(module, "InitialiseModule");

            if (moduleInstantiation)
            {
                Module* newModule = moduleInstantiation();
                newModule->name = moduleName;
                newModule->library = module;
                newModule->StartupModule();

                modules.Add(newModule);

                return true;
            }
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
            if (!FreeLibrary(module->library))
            {
                MR_LOG(LogModuleManager, Error, "FreeLibrary returned: %ls (%ls)", L"" /**Platform::GetError()*/, moduleName.Chr());
                return false;
            }
#else
            // FIXME: No non-windows implementation for module load!
#endif // MR_PLATFORM_WINDOWS

            modules.Pop(i);
        }

        return true;
    }

    return true;
}

bool ModuleManager::IsModuleLoaded(const String& moduleName)
{
    for (Module*& mdl : modules)
    {
        if (!mdl) continue;

        if (mdl->GetName() == moduleName)
        {
            return mdl->moduleState == ELoadState::ENABLED;
        }
    }

    return false;
}
