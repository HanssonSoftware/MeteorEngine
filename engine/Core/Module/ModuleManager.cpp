/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "ModuleManager.h"
#include <HAL/HAL.h>
#include <Application/Application.h>
#include <Special/EngineConstants.h>
#ifdef MR_PLATFORM_WINDOWS
#include <Win32/MinimalWin.h>
#include <libloaderapi.h>
#endif // MR_PLATFORM_WINDOWS
#pragma warning(disable : 6387)

LOG_ADDCATEGORY(ModuleManager);


ModuleManager& ModuleManager::Get()
{
    static ModuleManager instance;
    return instance;
}

void ModuleManager::InitializeModules()
{
}

void ModuleManager::ShutdownModules()
{
    for (IModule*& mdl : modules)
    {
        if (!mdl)
            continue;

        mdl->ShutdownModule();
    }
}

typedef IModule* (*fv)();

bool ModuleManager::RegisterModule(const StringView& moduleName)
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
            IModule* newModule = moduleInstantiation();
            newModule->name = moduleName;
            newModule->interface = module;
            MR_LOG(LogModuleManager, Log, "New module loaded: %hs", moduleName.ptr);
            newModule->StartupModule();

            modules[(char*)moduleName.ptr] = newModule;

            return true;
        }
    }

#endif // MR_PLATFORM_WINDOWS


    return false;
}

bool ModuleManager::UnloadModule(const StringView& moduleName)
{
    for (IModule*& mdl : modules)
    {
        if (mdl)
        {
            if (mdl->GetName().Chr() != (char*)moduleName.ptr)
                continue;

            mdl->ShutdownModule();
        }

        return true;
    }

    return true;
}

bool ModuleManager::IsModuleLoaded(const StringView& moduleName)
{
    for (IModule*& mdl : modules)
    {
        if (!mdl) continue;

        if (mdl->GetName().Chr() == (char*)moduleName.ptr)
        {
            return mdl->moduleState == IModule::ModuleState::Enabled;
        }
    }

    return false;
}

void ModuleManager::UpdateModules(float dt)
{
    for (IModule*& module : modules)
    {
        if (module->GetModuleState() != IModule::ModuleState::Running)
            continue;

        module->Update();
    }
}
