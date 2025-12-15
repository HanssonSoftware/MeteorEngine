/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <WindowManager/WindowManager.h>
#include <Windows.h>

class WindowsWindowManager : public IWindowManager
{
public:
	virtual ~WindowsWindowManager() noexcept;

	virtual void Init() override;

	virtual void Shutdown() override;

	virtual bool Present() override;

	HINSTANCE GetInstance() const { return instance; }

	virtual IWindow* GetFocusedWindow() override;

	bool GetIsUsingFallbackClass() const { return bIsUsingFallbackClassName; };

	virtual bool CreateWindow(const String& name, const Vector2<uint32_t> size, bool bShowOnSuccess = true) override;

	virtual IWindow* FindHWNDCorresponding(const HWND hWnd);

protected:
	inline bool RegisterWindowClass();

	bool bIsWinAPIClassRegistered = false;

	bool bIsUsingFallbackClassName = false;

	HINSTANCE instance;
};

LRESULT CALLBACK MeteorSpecifiedWindowProcedure(HWND, UINT, WPARAM, LPARAM);