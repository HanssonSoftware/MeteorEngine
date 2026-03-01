/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "Logging/Log.h"

#include "Commandlet.h"
#include <cstdint>
// #include <CoreProxy.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

LOG_ADDCATEGORY(Application);

struct CORE_API Application
{
	enum class ECurrentApplicationState { NONE, STARTUP, RUNNING, RESTARTING, SHUTDOWN, DEAD };

	Application() { appFramework = this; };

	Application(const Application&) = delete;

	virtual ~Application() noexcept = default;

	virtual void Init();

	virtual void Run();

	virtual void Shutdown();

	const char* GetApplicationName() const { return appName; };

	const char* GetApplicationCodeName() const { return appCodeName; };

	const char* GetApplicationNameNoSpaces() const { return appNameNoSpaces; };

	static void RequestExit(int Code);

	int GetRequestExitCode() const { return appFramework->exitCode; };

	ECurrentApplicationState GetAppState() const { return state; };

	void SetAppState(const ECurrentApplicationState& newState) { state = newState; };

	static Application& Get() { return *appFramework; }
protected:
	struct
	{
		/** Application name, this would be appearing on the created window. */
		const char* appName = nullptr;
		const char* appNameNoSpaces = nullptr;

		/** Useful for directories. */
		const char* appCodeName = nullptr;
	};

	int exitCode = 0;

	ECurrentApplicationState state = ECurrentApplicationState::NONE;

	static inline Application* appFramework = nullptr;
};

template<typename T = Application>
static T* GetApplication()
{
	return (T*)&Application::Get();
}

//#define IMPLEMENT_APPLICATION(ApplicationClass) \
