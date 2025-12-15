/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "WindowManager.h"
#include <WindowManager/Window.h>

IWindowManager::IWindowManager()
{
	inputManager = nullptr;
}

void IWindowManager::Init()
{
	
}

void IWindowManager::Shutdown()
{
	for (IWindow*& window : activeWindows)
	{
		delete window;
		window = nullptr;
	}
}