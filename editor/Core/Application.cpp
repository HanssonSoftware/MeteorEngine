/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"

#include <Platform/Winapi.h>
#include <Types/Map.h>

#include <Types/Array.h>
#include <Memory/MemoryBlockArena.h>

EditorApplication::EditorApplication() 
	: Application()
{
	appName = "Meteor Editor";
	appNameNoSpaces = "MeteorEditor";
	appCodeName = "Astronaut";
}

void EditorApplication::Init()
{
	Application::Init();

}

void EditorApplication::Run()
{	
	Application::Run();
}

void EditorApplication::Shutdown()
{

	Application::Shutdown();
}

#include <Special/LaunchMeteorSuite.h>

IMPLEMENT_WINDOWS_STARTUP("MeteorEngine-Core", EditorApplication);