/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <cstdint>
#include <Types/Delegate.h>
#include <Types/StringView.h>

#include "Logging/Log.h"
#include <HAL/Commandline.h>
#include <HAL/HAL.h>

// #include <CoreProxy.h>

#ifdef MR_CORE_EXPORT
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

LOG_ADDCATEGORY(Application);

struct CORE_API Application
{
	friend void HAL::InitEssential();

	enum class State : i8 
	{ 
		Dead = -1, 
		None, 
		PreStartup, 
		Startup, 
		PostStartup, 
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

	Application::State GetCurrentState() const { return currentState; };

	void SetCurrentState(const State newState) { currentState = newState; };

	const String* GetApplicationName() const { return &appName; };

	const String* GetApplicationCodeName() const { return &appCodeName; };

	const String* GetApplicationNameNoSpaces() const { return &appNameNoSpaces; };

	Commandline* GetCommandline() { return &cli; };

	void Tick();
protected:

	State currentState = { State::None };

	//* Application name, this would be appearing on the created window
	String appName;
	String appNameNoSpaces;

	//* Useful for directories
	String appCodeName;

	bool bUseSplash = false;

	Commandline cli;
};

template<typename T = Application>
static T* GetApplication()
{
	return (T*)Application::Get();
}

//#define IMPLEMENT_APPLICATION(ApplicationClass) \
