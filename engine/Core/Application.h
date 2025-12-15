/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include "Logging/Log.h"

#include "Commandlet.h"
#include <cstdint>

LOG_ADDCATEGORY(Application);

struct Application
{
	enum class ECurrentApplicationState { NONE, STARTUP, RUNNING, RESTARTING, SHUTDOWN, DEAD };

	Application();

	Application(const Application&) = delete;

	virtual ~Application() noexcept = default;

	virtual void Init();

	virtual void Run();

	virtual void Shutdown();

	const String GetApplicationName() const { return appName; };

	const String GetApplicationCodeName() const { return appCodeName; };

	const String GetApplicationNameNoSpaces() const { return appNameNoSpaces; };

	static void RequestExit(int32_t Code);

	int32_t GetRequestExitCode() const { return appFramework->exitCode; };

	ECurrentApplicationState GetAppState() const { return state; };

	void SetAppState(const ECurrentApplicationState& newState) { state = newState; };

	static Application& Get() { return *appFramework; }
protected:
	struct
	{
		/** Application name, this would be appearing on the created window. */
		String appName;
		String appNameNoSpaces;

		/** Useful for directories. */
		String appCodeName;
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

#define IMPLEMENT_APPLICATION(ApplicationClass) \
	/*extern "C" __declspec(dllexport)*/ int LaunchApplication(int ArgumentCount, char* Arguments[]) \
	{	\
		static ApplicationClass instance; \
		Commandlet::Initialize(); \
        instance.Init(); \
        instance.Run(); \
        instance.Shutdown(); \
        return instance.GetRequestExitCode(); \
	}