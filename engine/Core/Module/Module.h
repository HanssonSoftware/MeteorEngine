/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
//#include <type_traits>

#include "HAL/Win32/MinimalWin.h"
// #include <CoreProxy.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API Module
{
	friend class ModuleManager;
protected:

	enum class ModuleState
	{
		None,
		Loading,
		Running,
		Enabled,
		Disabled,
		Unloaded
	};

public:
	Module() = default;
	virtual ~Module() noexcept = default;

	Module(const Module&) = delete;
	Module(Module&&) = delete;
	
	Module& operator=(const Module&) = delete;
	Module& operator=(Module&&) = delete;

	virtual bool StartupModule() = 0;

	virtual void ShutdownModule() = 0;

	virtual void Update() = 0;

	String GetName() const { return name; }

	ModuleState GetModuleState() const { return moduleState; };

protected:
	ModuleState moduleState = ModuleState::None;

	String name;
};

#define IMPLEMENT_MODULE(ModuleClass)																		 \
	extern "C" __declspec(dllexport) Module* InitialiseModule()												 \
	{									\
		return new ModuleClass();																			 \
	}
