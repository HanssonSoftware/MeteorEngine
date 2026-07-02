/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"

#include "Win32/MinimalWin.h"
#include <Types/Map.h>
#include <HAL/Window.h>

#include <Types/Array.h>
#include <Memory/MemoryBlockArena.h>

EditorApplication::EditorApplication() 
	: Application()
{
	appName = { "Meteor Editor" };
	appNameNoSpaces = { "MeteorEditor" };
	appCodeName = { "Satellite" };
}

void EditorApplication::Init()
{
	Application::Init();

	Window splash({ "" }, 3, 32);

	cli.Check("-maxram");
	auto a = cli.Get("-maxram");

	int J = 4;

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

IMPLEMENT_APPLICATION_STARTUP("core", EditorApplication);