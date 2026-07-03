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

	Window* splash = Window::Create({ "valami" }, 620, 300, GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);
	splash->Show();

	cli.Check("-maxram");
	auto a = cli.Get("-maxram");

	int J = 4;

	splash->Destroy();
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