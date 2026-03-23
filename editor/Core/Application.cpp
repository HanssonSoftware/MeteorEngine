/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"

#include <Platform/Winapi.h>
#include <uxtheme.h>
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "UxTheme.lib")

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

	String a = "kurvsyékddfága";
	
	MemoryBlockArena<int> ar = {16 * 1024 * 1024 };

	int* j = (int*)ar.Allocate(sizeof(int));

	*j = 16;
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