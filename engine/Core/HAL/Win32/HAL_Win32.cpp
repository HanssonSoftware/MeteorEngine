/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS

#include <HAL/HAL.h>
#include <Logging/Log.h>
#include "MinimalWin.h"
#include <stringapiset.h>
#include <Application/Application.h>

LOG_ADDCATEGORY(HAL);

static LRESULT CALLBACK GlobalWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		GetApplication()->SetCurrentState(Application::State::Shutdown);
		GetApplication()->Shutdown();
		return 0;
	default:
		return DefWindowProcW(hwnd, msg, wparam, lparam);
	}

	return 0;
}

namespace HAL
{
	bool ConvertToWide(wchar_t* targetBuffer, const u32 size, const char* convertibleBuffer)
	{
		if (size && targetBuffer && convertibleBuffer)
		{
			MultiByteToWideChar(CP_UTF8, 0, convertibleBuffer, size, targetBuffer, size);
			return true;
		}

		MR_LOG(LogHAL, Error, "Conversion error! MultiByteToWideChar[%d:%s]", GetLastError(), LocalizeErrorCode(GetLastError()));
		return false;
	}

	bool ConvertToNarrow(char* targetBuffer, const u32 size, const wchar_t* convertibleBuffer)
	{
		if (size && targetBuffer && convertibleBuffer)
		{
			WideCharToMultiByte(CP_UTF8, 0, convertibleBuffer, size, targetBuffer, size, nullptr, nullptr);
			return true;
		}

		MR_LOG(LogHAL, Error, "Conversion error! WideCharToMultiByte[%d:%s]", GetLastError(), LocalizeErrorCode(GetLastError()));
		return false;
	}

	String LocalizeErrorCode(i64 code)
	{
		wchar_t final[512] = {};

		const DWORD count = FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			(DWORD)code,
			LANG_USER_DEFAULT,
			final,
			511,
			nullptr
		);

		if (count == 0)
		{
			MR_LOG(LogHAL, Error, "Conversion error! WideCharToMultiByte[%d:%s]");
			return { "" };
		}

		char end[512] = {};
		ConvertToNarrow(end, count - 2, final);

		return { end, count - 2 };
	}

	bool PeekOSMessageQueue()
	{
		i32 returnStatement = 0;
		MSG msg;
		while ((returnStatement = GetMessageW(&msg, GetActiveWindow(), 0, 0)) != 0)
		{
			if (returnStatement == -1)
			{
				return false;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
				return true;
			}
		}

		return false;
	}

	void InitEssential()
	{
		const String* codeName = GetApplication()->GetApplicationCodeName();

		wchar_t buffer[256] = {};
		MultiByteToWideChar(CP_UTF8, 0, (char*)codeName->Chr(), codeName->Length(), buffer, codeName->Length());

		WNDCLASSEXW registerData = {};
		registerData.cbSize = sizeof(WNDCLASSEXW);
		registerData.hInstance = GetModuleHandleW(nullptr);
		registerData.lpfnWndProc = GlobalWndProc;
		registerData.lpszClassName = buffer;

		if (!RegisterClassExW(&registerData))
		{
			MR_LOG(LogApplication, Fatal, "Unable to register engine critical component to Windows! RegisterClassExW=%d", GetLastError());
		}

		if (GetApplication()->bUseSplash)
		{

		}
	}

	void ShutdownEssential()
	{
		const String* codeName = GetApplication()->GetApplicationCodeName();

		wchar_t buffer[256] = {};
		MultiByteToWideChar(CP_UTF8, 0, (char*)codeName->Chr(), codeName->Length(), buffer, codeName->Length());

		if (!UnregisterClassW(buffer, GetModuleHandleW(nullptr)))
		{
			MR_LOG(LogApplication, Fatal, "Failed to unregister engine critical component from Windows! UnregisterClassW=%d", GetLastError());
		}
	}
}

#endif // MR_PLATFORM_WINDOWS