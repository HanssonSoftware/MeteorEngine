/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <WindowManager/Window.h>
#include <Windows.h>

//class WindowsWindowManager;

class WindowsWindow : public IWindow
{
	friend class WindowsWindowManager;
public:
	WindowsWindow(WindowsWindowManager* owner);

	virtual ~WindowsWindow() noexcept = default;

	virtual void* GetWindowHandle() override;

protected:
	HWND handle;
};

using Window = WindowsWindow;