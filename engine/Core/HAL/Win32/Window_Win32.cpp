/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include <HAL/Window.h>
#include <Logging/Log.h>

#include "MinimalWin.h"

bool Window::CallOSInsider()
{


	return true;
}

Window* Window::CreateWindow(const StringView& name, const u32 sx, const u32 sy, const u32 px, const u32 py)
{
	return nullptr;
}

#endif // MR_PLATFORM_WINDOWS

