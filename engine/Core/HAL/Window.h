/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/StringView.h>
#include <Types/Vector.h>

class Window
{
public:
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

protected:

	bool CallOSInsider();

	void* windowsAPIHandle = nullptr;

	StringView windowName;
	
	Vector2<u32> windowSize;
};

