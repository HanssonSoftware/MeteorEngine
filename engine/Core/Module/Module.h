/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
//#include <type_traits>

#include <Platform/Winapi.h>
// #include <CoreProxy.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

enum class ELoadState
{
	NONE,
	LOADING,
	ENABLED,
	DISABLED,
	UNLOADED
};

class CORE_API Module
{
	friend class ModuleManager;
public:
	virtual void StartupModule() = 0;

	virtual void ShutdownModule() = 0;

	virtual void Update() = 0;

	String GetName() const { return name; }

protected:
#ifdef MR_PLATFORM_WINDOWS
	//HMODULE library;
#endif // MR_PLATFORM_WINDOWS

	ELoadState moduleState;

	String name;
};

#define IMPLEMENT_MODULE(ModuleClass)																		 \
	static_assert(std::is_base_of<Module, ModuleClass>::value, "ModuleClass does not inherit from Module!"); \
	extern "C" __declspec(dllimport) Module* InitialiseModule()												 \
	{																										 \
		return new ModuleClass();																			 \
	}
