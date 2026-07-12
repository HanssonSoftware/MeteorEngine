/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"

#include "Win32/MinimalWin.h"
#include <Types/Map.h>
#include <HAL/Window.h>
#include <HAL/HAL.h>
#include <HAL/Paths.h>

#include <Types/Array.h>
#include <Memory/MemoryBlockArena.h>

EditorApplication::EditorApplication() 
	: Application()
{
	appName = { "Meteor Editor fdg" };
	appNameNoSpaces = "MeteorEditor";
	appCodeName = { "Satellite" };

}

void EditorApplication::Init()
{
	Application::Init();

	cli.Check("-maxram");
	auto a = cli.Get("-maxram");

	int J = 4;

	editorWindow = Window::Create({ "Editor" }, 300, 300, 1600, 900);
	editorWindow->Show();
}

void EditorApplication::Run()
{	
	Application::Run();

	int J = 35;
}

void EditorApplication::Shutdown()
{

	Application::Shutdown();
}


#include <Special/LaunchMeteorSuite.h>

IMPLEMENT_APPLICATION_STARTUP("core", EditorApplication);