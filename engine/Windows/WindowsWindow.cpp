/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "WindowsWindow.h"
#include <Logging/Log.h>
#include <Windows/WindowsWindowManager.h>
//#include <../Resources/resource.h>
//#include <dwmapi.h>
#include <winuser.h>
#include <Application.h>

#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "UxTheme.lib")

#ifdef MR_DEBUG
static constexpr bool bIsDebug = true;
#else
static constexpr bool bIsDebug = false;
#endif // MR_DEBUG


WindowsWindow::WindowsWindow(WindowsWindowManager* owner) 
	: IWindow(owner)
{

}

void* WindowsWindow::GetWindowHandle()
{
	return (HWND)handle;
}
