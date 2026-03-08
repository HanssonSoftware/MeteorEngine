/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"
#include <Special/LaunchMeteorSuite.h>


#include <Types/Array.h>
//#include <Memory/MemoryBlockArena.h>

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


	Array<int> valami;

	for (int i = 0; i < 10; i++)
	{
		valami.Add(i + 4);
	}
	
	//MemoryBlockArena<int> ar = {16 * 1024 * 1024 };

	//int* j = (int*)ar.Allocate(sizeof(int));

	//*j = 16;

	//ar.Reset();

	for (int& a : valami)
		printf("%d", a);
}

void EditorApplication::Run()
{	
	Application::Run();
}

void EditorApplication::Shutdown()
{
	Application::Shutdown();
}

IMPLEMENT_WINDOWS_STARTUP("MeteorEngine-Core", EditorApplication);