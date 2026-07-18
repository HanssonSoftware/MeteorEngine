/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS

#include <HAL/HAL.h>
#include <Logging/Log.h>
#include "MinimalWin.h"
#include <stringapiset.h>
#include <Application/Application.h>
#include <Types/String.h>

#include <DefaultEngineResources.h>

LOG_ADDCATEGORY(HAL);

static HMODULE engine = nullptr;

static LONG CrashHandler(_EXCEPTION_POINTERS* ExceptionInfo)
{
	MessageBoxW(nullptr, L"Engine crashed!", L"", MB_OK);

	return 1;
}

static LRESULT CALLBACK GlobalWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		GetApplication()->SetCurrentState(Application::State::Shutdown);
		return 0;
	case WM_ERASEBKGND:
		return 1;

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

		MR_LOG(LogHAL, Error, "Conversion error! MultiByteToWideChar[%d:%s]", GetLastError(), (wchar_t*)LocalizeErrorCode(GetLastError()));
		return false;
	}

	bool ConvertToNarrow(char* targetBuffer, const u32 size, const wchar_t* convertibleBuffer)
	{
		if (size && targetBuffer && convertibleBuffer)
		{
			WideCharToMultiByte(CP_UTF8, 0, convertibleBuffer, size, targetBuffer, size, nullptr, nullptr);
			return true;
		}

		MR_LOG(LogHAL, Error, "Conversion error! WideCharToMultiByte[%d:%s]", GetLastError(), (wchar_t*)LocalizeErrorCode(GetLastError()));
		return false;
	}

	void* LocalizeErrorCode(i64 code)
	{
		thread_local static wchar_t final[512] = {};

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
			return nullptr;
		}

		return final;
	}

	bool PeekOSMessageQueue()
	{
		i32 returnStatement = 0;
		MSG msg;
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
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

		return true;
	}

	void InitHAL()
	{
		GetApplication()->applicationHandle = GetModuleHandleW(nullptr);

		AddVectoredExceptionHandler(1, CrashHandler);

		const String* codeName = GetApplication()->GetApplicationCodeName();

		wchar_t buffer[256] = {};
		MultiByteToWideChar(CP_UTF8, 0, (char*)codeName->Chr(), codeName->Length(), buffer, codeName->Length());

		HICON def = (HICON)LoadImageW(
			engine,
			MAKEINTRESOURCEW(IDI_LASTRESORT),
			IMAGE_ICON,
			0, 0,
			LR_DEFAULTSIZE | LR_SHARED);

		if (!def)
		{
			MR_LOG(LogHAL, Warn, "Unable to querry icons from fallback! LoadImageW=%d", GetLastError());
		}


		WNDCLASSEXW registerData = {};
		registerData.cbSize = sizeof(WNDCLASSEXW);
		registerData.hInstance = engine;
		registerData.lpfnWndProc = GlobalWndProc;
		registerData.lpszClassName = buffer;
		registerData.hIcon = registerData.hIconSm = def;

		if (!RegisterClassExW(&registerData))
		{
			MR_LOG(LogApplication, Fatal, "Unable to register engine critical component to Windows! RegisterClassExW=%d", GetLastError());
		}
	}

	void ShutdownEssential()
	{
		const String* codeName = GetApplication()->GetApplicationCodeName();

		wchar_t buffer[256] = {};
		MultiByteToWideChar(CP_UTF8, 0, (char*)codeName->Chr(), codeName->Length(), buffer, codeName->Length());

		if (!UnregisterClassW(buffer, (HINSTANCE)HAL::GetEngineCore()))
		{
			MR_LOG(LogApplication, Fatal, "Failed to unregister engine critical component from Windows! UnregisterClassW=%d", GetLastError());
		}

		RemoveVectoredExceptionHandler(CrashHandler);
	}

	u32 FatalExit(u32 code)
	{
		ExitProcess((UINT)code);
		return code;
	}

	void* GetEngineCore()
	{
		if (!engine)
		{
			if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&GlobalWndProc, &engine))
			{
				MR_LOG(LogHAL, Fatal, "Failed to querry critical resources! GetModuleHandleExW=%d", GetLastError());
				return nullptr;
			}
		}

		return engine;
	}

#ifdef MR_DEBUG
	void DebugBreak()
	{
		::DebugBreak();
	}
#endif // MR_DEBUG
}

#endif // MR_PLATFORM_WINDOWS