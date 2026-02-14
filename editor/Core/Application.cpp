/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"
#include <Resource/MemoryManager.h>
#include <Types/String.h>
#include <iostream>
#include <Platform/File.h>
#include <Types/Array.h>
#include <Commandlet.h>
#include <Platform/FileManager.h>
#include "EditorLayer.h"
#include <Platform/PerformanceTimer.h>
#include <Types/Pointers.h>
#include <Platform/Paths.h>
#include <Platform/Platform.h>
#include <Special/LaunchMeteorSuite.h>

EditorApplication::EditorApplication() 
	: Application()
{
	appName = TEXT("Meteor Editor");
	appNameNoSpaces = TEXT("MeteorEditor");
	appCodeName = TEXT("Astronaut");

}

void EditorApplication::Init()
{
	Application::Init();
	

	Application::Run();
}

void EditorApplication::Run()
{	
	
}

void EditorApplication::Shutdown()
{
	Application::Shutdown();
}

IMPLEMENT_WINDOWS_STARTUP("MeteorEngine-Core", EditorApplication);