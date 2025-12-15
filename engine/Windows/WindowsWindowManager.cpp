/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "WindowsWindowManager.h"
//#include <../Resources/resource.h>
#include <Application.h>
#include "WindowsWindow.h"
#include <dwmapi.h>
#include <Module/ModuleManager.h>

#include <Platform/Platform.h>

#pragma comment (lib, "UxTheme.lib")

void WindowsWindowManager::Init()
{
    IWindowManager::Init();

    if (!RegisterWindowClass())
    {
        MR_LOG(LogWindowManager, Fatal, "Failed to register window class!")
    }
}

WindowsWindowManager::~WindowsWindowManager() noexcept
{
    IWindowManager::~IWindowManager();
}


void WindowsWindowManager::Shutdown()
{
    IWindowManager::Shutdown();

    if (bIsWinAPIClassRegistered)
    {
        //GetApplication()->GetApplicationNameNoSpaces();
        UnregisterClassW(nullptr, instance);
    }
}

bool WindowsWindowManager::Present()
{


    return true;
}

bool WindowsWindowManager::CreateWindow(const String& name, const Vector2<uint32_t> size, bool bShowOnSuccess)
{
    if (size < 8)
        return false;

    WindowsWindow* instance = new WindowsWindow(this);
    instance->windowTitle = name;
    instance->windowSize = size;

    HINSTANCE Inst = GetModuleHandle(nullptr);

    RECT windowRect = { 0, 0, (LONG)size.x, (LONG)size.y };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, 0);

    //instance->handle = ::CreateWindowExW(
    //    /*WS_EX_ACCEPTFILES*/ 0,
    //    GetApplication()->GetApplicationNameNoSpaces(),
    //    name,
    //    WS_OVERLAPPEDWINDOW,
    //    CW_USEDEFAULT,
    //    CW_USEDEFAULT,
    //    windowRect.right - windowRect.left,
    //    windowRect.bottom - windowRect.top,
    //    0,
    //    0,
    //    Inst,
    //    0
    //);

    if (!instance->handle)
    {
        MR_LOG(LogWindowManager, Error, "CreateWindowExW returned: %ls", *Platform::GetError());
        return false;
    }

    static const constexpr BOOL bCanIUseDarkWindowTitlebar = 1;
    DwmSetWindowAttribute(instance->handle, DWMWA_USE_IMMERSIVE_DARK_MODE, &bCanIUseDarkWindowTitlebar, sizeof(bCanIUseDarkWindowTitlebar));

    activeWindows.Add(instance);

    if (bShowOnSuccess)
    {
        ::ShowWindow(instance->handle, SW_SHOWDEFAULT);
    }

    return true;
}

IWindow* WindowsWindowManager::FindHWNDCorresponding(const HWND hWnd)
{
    for (IWindow* window : activeWindows)
    {
        if ((HWND)window->GetWindowHandle() == hWnd)
            return window;
    }

    return nullptr;
}

inline bool WindowsWindowManager::RegisterWindowClass()
{
	instance = GetModuleHandle(nullptr);
	//HICON ico = (HICON)LoadImage(instance, MAKEINTRESOURCE(IDI_DEFAULTAPPICON), IMAGE_ICON, 64, 64, LR_DEFAULTCOLOR);

    const String className = GetApplication()->GetApplicationNameNoSpaces();

	WNDCLASSEXW windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hInstance = instance;
	//windowClass.hIcon = ico;
	//windowClass.hIconSm = ico;
	windowClass.lpfnWndProc = MeteorSpecifiedWindowProcedure;
    //windowClass.lpszClassName = className;

	if (!RegisterClassExW(&windowClass))
	{
		MR_LOG(LogWindowManager, Fatal, "Failed to register WinAPI class!");
		return false;
	}

	bIsWinAPIClassRegistered = true;
	return true;
}

IWindow* WindowsWindowManager::GetFocusedWindow()
{
    return FindHWNDCorresponding(GetFocus());
}

LRESULT CALLBACK MeteorSpecifiedWindowProcedure(HWND wnd, UINT uint, WPARAM p1, LPARAM p2)
{
    switch (uint)
    {
    case WM_CREATE:
        // OutputDebugString(L"Create\n");
        break;

        //case WM_PAINT:
        //    break;

    case WM_SYSCOMMAND:
        break;

    case WM_SIZE:
        break;

    case WM_DESTROY:
        
        break;

    case WM_CLOSE:
        Application::RequestExit(0);
        break;
    case WM_SIZING:
        break;

    case WM_WINDOWPOSCHANGING:
        break;

    case WM_WINDOWPOSCHANGED:
        break;
    case WM_KEYDOWN:
        break;
    case WM_MOVING:
        break;
    default:
        return DefWindowProc(wnd, uint, p1, p2);
    }

    return DefWindowProc(wnd, uint, p1, p2);
}
