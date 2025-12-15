/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <Types/Vector.h>
#include <stdint.h>

class IWindow
{
	friend class IWindowManager;
public:

	IWindow(IWindowManager* newOwner);

	virtual ~IWindow() noexcept = default;

	Vector2<uint32_t> GetSize() const
	{
		return windowSize;
	}

	virtual void* GetWindowHandle() = 0;

protected:
	IWindowManager* owner;

	Vector2<uint32_t> windowSize;

	String windowTitle;
};

#include <Platform/PlatformLayout.h>

#ifdef MR_PLATFORM_WINDOWS
#include <Windows/WindowsWindow.h>
#endif // MR_PLATFORM_WINDOWS


