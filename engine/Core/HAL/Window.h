/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/StringView.h>
#include <Types/String.h>

#ifdef MR_CORE_EXPORT
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

class CORE_API Window
{
public:
	static Window* Create(const StringView& name, const u32 sx, const u32 sy, const u32 px, const u32 py);

	void Show();

	void Hide();

	void Rename(const StringView& newName);

	void Resize(const u32 x, const u32 y);

	void Destroy();

	void* GetNativeHandle() const { return windowsAPIHandle; };
protected:

	Window() noexcept = default;
	Window(const StringView& name, const u32 x, const u32 y)
		: windowName(name)
	{
		this->x = x;
		this->y = y;
	}

	Window(const Window& old)
		: windowName(old.windowName)
		, x(old.x)
		, y(old.y)
	{

	}

	bool CallOSInsider();

	void* windowsAPIHandle = nullptr;

	String windowName;
	
	u32 x, y;
};

