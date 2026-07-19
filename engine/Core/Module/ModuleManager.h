/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Map.h>
#include <Types/String.h>
#include "Module.h"

#ifdef MR_CORE_EXPORT
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API ModuleManager
{
	ModuleManager() noexcept = default;
public:
	static ModuleManager& Get();

	virtual void InitializeModules();
	
	virtual void ShutdownModules();

	virtual bool RegisterModule(const StringView& moduleName);

	virtual bool UnloadModule(const StringView& moduleName);

	virtual bool IsModuleLoaded(const StringView& moduleName);

	virtual void UpdateModules(float dt);
protected:
	virtual ~ModuleManager() noexcept = default;

	Map<const char*, IModule*> modules;

private:
	static ModuleManager* object; // This must be an extensible class!
};

