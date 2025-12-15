/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Window.h"
#include "WindowManager.h"


IWindow::IWindow(IWindowManager* newOwner)
	: owner(newOwner)
{

}