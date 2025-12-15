/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include "Window.h"
#include <Object/Object.h>
#include <Logging/Log.h>
#include <Types/Array.h>

LOG_ADDCATEGORY(WindowManager);

#undef CreateWindow

class InputManager;

inline static constexpr const wchar_t* GetDefaultApplicationName() noexcept
{
	return L"MeteorApplication";
}

class IWindowManager
{
public:
	IWindowManager();

	virtual ~IWindowManager() noexcept = default;

	virtual void Init();

	virtual void Shutdown();

	virtual bool Present() = 0;

	virtual bool CreateWindow(const String& name, const Vector2<uint32_t> size, bool bShowOnSuccess = true) = 0;

	/** Gets the current or last focused window, can be non-engine specific. */
	virtual IWindow* GetFocusedWindow() = 0;

	InputManager* GetInputManager() const { return inputManager; };
protected:

	InputManager* inputManager;

	Array<IWindow*> activeWindows;
};


#include <Platform/PlatformLayout.h>

#ifdef MR_PLATFORM_WINDOWS
#include <Windows/WindowsWindowManager.h>
#endif // MR_PLATFORM_WINDOWS

