/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Application.h"
#include <Types/String.h>
#include <MemoryManager.h>
#include <Platform/FileManager.h>

#include <Spec/LaunchMeteorSuite.h>
#include <Platform/Platform.h>

//#pragma comment(lib, "Shell32.lib")


BuildSystemApplication::BuildSystemApplication()
	: Application()
{
	appName = "Meteor Build";
	appNameNoSpaces = "MeteorBuild";
	appCodeName = "Apollo";
}

void BuildSystemApplication::Init()
{
	if (!GetBuildSystem().InitFramework())
	{
		MR_LOG(LogBuildSystemApplication, Fatal, "Build system error!");
	}

	Application::Init();
}

void BuildSystemApplication::Run()
{
	// Application::RequestExit(0); should be added somewhere, if not the app will loop over this function

	GetBuildSystem().OrderModules();
	if (!GetBuildSystem().BuildProjectFiles())
	{
		MR_LOG(LogBuildSystemApplication, Fatal, "Failed to create project files!");
	}

	Application::RequestExit(0);
}

void BuildSystemApplication::Shutdown()
{
	Application::Shutdown();
}

IMPLEMENT_WINDOWS_STARTUP("MeteorEngine-Core.dll", BuildSystemApplication);
