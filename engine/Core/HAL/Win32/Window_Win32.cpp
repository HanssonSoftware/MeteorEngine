/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include <HAL/Window.h>
#include <HAL/HAL.h>
#include <Logging/Log.h>
#include <Memory/MemoryHandler.h>
#include <Application/Application.h>

#include "MinimalWin.h"
#include <dwmapi.h>

#pragma comment(lib, "Dwmapi.lib")

LOG_ADDCATEGORY(Windowing);

Window* Window::Create(const StringView& name, const u32 sx, const u32 sy, const u32 px, const u32 py)
{
	const bool bApplicationCodeNameTooBig = GetApplication()->GetApplicationCodeName()->Length() > 64;
	const bool bNewWindowNameTooBig = name.size > 256;

	if (bApplicationCodeNameTooBig && bNewWindowNameTooBig)
	{
		MR_LOG(LogWindowing, Warn, "Both CodeName and New Window Name are too big!");
		return nullptr;
	}
	else if (bApplicationCodeNameTooBig && !bNewWindowNameTooBig)
	{
		MR_LOG(LogWindowing, Warn, "CodeName is too big!");
		return nullptr;
	}
	else if (!bApplicationCodeNameTooBig && bNewWindowNameTooBig)
	{
		MR_LOG(LogWindowing, Warn, "Window name is too big!");
		return nullptr;
	}

	wchar_t className[64] = {};
	HAL::ConvertToWide(className, GetApplication()->GetApplicationCodeName()->Length(), (char*)GetApplication()->GetApplicationCodeName()->Chr());

	wchar_t windowName[256] = {};
	HAL::ConvertToWide(windowName, name.size, (char*)name.ptr);

	HWND winHandle = ::CreateWindowExW(WS_EX_ACCEPTFILES, className, windowName, WS_OVERLAPPEDWINDOW, sx, sy, px, py, nullptr, nullptr, (HINSTANCE)HAL::GetEngineCore(), nullptr);

	static constexpr const COLORREF darkColor = 0x00151515;

	DwmSetWindowAttribute(
		winHandle,
		DWMWA_USE_IMMERSIVE_DARK_MODE,
		&darkColor,
		sizeof(darkColor)
	);


	Window* newWindowHandle = GetMemoryManager()->Allocate<Window>(sizeof(Window), GetMemoryManager()->GetProjectRegion());
	newWindowHandle->windowName = name;
	newWindowHandle->x = sx;
	newWindowHandle->y = sy;
	newWindowHandle->windowsAPIHandle = winHandle;

	return newWindowHandle;
}

void Window::Show()
{
	if (!windowsAPIHandle)
	{
		MR_LOG(LogWindowing, Error, "OS Specific handle is invalid for window: %s", windowName.Chr());
		return;
	}

	::ShowWindow((HWND)windowsAPIHandle, SW_SHOWDEFAULT);
}

void Window::Hide()
{
	if (!windowsAPIHandle)
	{
		MR_LOG(LogWindowing, Error, "OS Specific handle is invalid for window: %s", windowName.Chr());
		return;
	}

	::ShowWindow((HWND)windowsAPIHandle, SW_HIDE);
}

void Window::Rename(const StringView& newName)
{
	if (!windowsAPIHandle)
	{
		MR_LOG(LogWindowing, Error, "OS Specific handle is invalid for window: %s", windowName.Chr());
		return;
	}

	wchar_t fixed[256] = {};
	const u32 realSize = newName.size > 255 ? 255 : newName.size;
	
	if (!HAL::ConvertToWide(fixed, realSize, (char*)newName.ptr))
	{
		MR_LOG(LogWindowing, Error, "Conversion error! MultiByteToWideChar=%d", GetLastError());
		return;
	}

	::SetWindowTextW((HWND)windowsAPIHandle, fixed);
}

void Window::Resize(const u32 x, const u32 y)
{
	this->x = x;
	this->y = y;

	/*if (!windowsAPIHandle)
	{
		MR_LOG(LogWindowing, Error, "OS Specific handle is invalid for window: %s", windowName.ptr);
	}*/
}

void Window::Destroy()
{
	if (!windowsAPIHandle)
	{
		MR_LOG(LogWindowing, Error, "OS Specific handle is invalid for window: %s", windowName.Chr());
		return;
	}

	::DestroyWindow((HWND)windowsAPIHandle);
}

#endif // MR_PLATFORM_WINDOWS

