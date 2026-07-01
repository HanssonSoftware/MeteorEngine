/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <cstdint>
#include <Types/Delegate.h>
#include <Types/StringView.h>

#include "Logging/Log.h"
#include <HAL/Commandline.h>
// #include <CoreProxy.h>

#ifdef MR_CORE_EXPORT
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

LOG_ADDCATEGORY(Application);

struct CORE_API Application
{
	enum class State : i8 
	{ 
		Dead = -1, 
		None, 
		Startup, 
		Running, 
		Restarting, 
		Shutdown 
	};

	static Application* Get();

	Application();

	Application(const Application&) = delete;

	Application(Application&&) = delete;

	virtual ~Application() noexcept = default;

	virtual void Init();

	virtual void Run();

	virtual void Shutdown();

	void RequestExit(u32 code);

	const StringView* GetApplicationName() const { return &appName; };

	const StringView* GetApplicationCodeName() const { return &appCodeName; };

	const StringView* GetApplicationNameNoSpaces() const { return &appNameNoSpaces; };

	Commandline* GetCommandline() { return &cli; };
protected:
	State currentState = { State::None };

	//* Application name, this would be appearing on the created window
	StringView appName;
	StringView appNameNoSpaces;

	//* Useful for directories
	StringView appCodeName;

	Commandline cli;

	//MemoryHandler memoryAllocator;
};

template<typename T = Application>
static T* GetApplication()
{
	return (T*)Application::Get();
}

//#define IMPLEMENT_APPLICATION(ApplicationClass) \
