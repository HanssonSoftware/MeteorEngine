/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"

#include "Win32/MinimalWin.h"
#include <Types/Map.h>
#include <HAL/Window.h>
#include <HAL/HAL.h>
#include <HAL/Paths.h>

#include <Types/Array.h>
#include <Memory/MemoryBlockArena.h>
#include <Module/ModuleManager.h>

EditorApplication::EditorApplication() 
	: Application()
{
	appName = { "Meteor Editor" };
	appNameNoSpaces = "MeteorEditor fdxghbydfgb";
	appCodeName = { "Satellite" };

	windowDefs = { appName, 640, 480 };

	int J = 53;
}

void EditorApplication::Init()
{
	Application::Init();

	cli.Check("-maxram");
	auto a = cli.Get("-maxram");
	ModuleManager::Get().RegisterModule("vulkan");
}

void EditorApplication::Run(float dt)
{	
	Application::Run(dt);

	int J = 35;
}

void EditorApplication::Shutdown()
{

	Application::Shutdown();
}


#include <Special/LaunchMeteorSuite.h>

IMPLEMENT_APPLICATION_STARTUP("core", EditorApplication);