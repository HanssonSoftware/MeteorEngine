/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/StringView.h>
#include <Types/Vector.h>

#undef CreateWindow

class Window
{
public:
	static Window* CreateWindow(const StringView& name, const u32 sx, const u32 sy, const u32 px, const u32 py);
protected:

	Window() noexcept = default;
	Window(const StringView& name, const u32 x, const u32 y)
		: windowName(name)
	{
		windowSize = { x, y };
	}

	Window(const Window& old)
		: windowName(old.windowName)
		, windowSize(old.windowSize)
	{

	}

	bool CallOSInsider();

	void* windowsAPIHandle = nullptr;

	StringView windowName;
	
	Vector2<u32> windowSize;
};

