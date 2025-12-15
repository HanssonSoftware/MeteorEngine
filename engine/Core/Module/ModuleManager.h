/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Types/String.h>
#include "Module.h"

class ModuleManager
{
	ModuleManager() noexcept;
public:
	static ModuleManager& Get();

	virtual bool LoadModule(const String& moduleName);

	virtual bool UnloadModule(const String& moduleName);

	virtual bool IsModuleLoaded(const String& moduleName);
protected:
	virtual ~ModuleManager() noexcept;

	Array<Module*> modules;

private:
	static ModuleManager* object; // This must be an extensible class!
};

