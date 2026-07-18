/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"
#include <Types/String.h>
#include <Memory/MemoryHandler.h>

#include <Special/LaunchMeteorSuite.h>
#include <HAL/HAL.h>

#include <shellapi.h>
//#pragma comment(lib, "Shell32.lib")


BuildSystemApplication::BuildSystemApplication()
	: Application()
{
	appName = "Meteor Build";
	appNameNoSpaces = "MeteorBuild";
	appCodeName = "Apollo";

	bHideWindow = true;
}

void BuildSystemApplication::Init()
{
	Application::Init();

	GetBuildSystem().CallRequiredMode();
}

void BuildSystemApplication::Run(float dt)
{

}

void BuildSystemApplication::Shutdown()
{
	Application::Shutdown();
}

IMPLEMENT_APPLICATION_STARTUP("core", BuildSystemApplication);