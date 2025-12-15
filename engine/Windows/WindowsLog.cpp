/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "WindowsLog.h"
#include <Windows.h>
#include <Types/String.h>
#include <Application.h>
#include "../Version.h"
#include "WindowsFile.h"
#include <Platform/Platform.h>

#include <Logging/LogAssertion.h>
#pragma warning(disable : 6001) // Using uninitialized memory 'X'

//LOG_ADDCATEGORY(Assertion);

static HANDLE hConsole;

static LONG ExceptionHandler(EXCEPTION_POINTERS* ptr)
{
	return 4;
}

WindowsLogger::WindowsLogger()
	: ILogger()
{
	
}

WindowsLogger::~WindowsLogger() noexcept
{

}

void WindowsLogger::Initialize()
{
#ifdef MR_DEBUG
	if constexpr (bIsRunningDebugMode)
	{
		FreeConsole();

		if (!AllocConsole())
		{
			if (GetLastError() == ERROR_ACCESS_DENIED)
			{
				if (!AttachConsole(ATTACH_PARENT_PROCESS))
				{
					Application::RequestExit(-1);
				}
			}
			else
			{
				Application::RequestExit(-1);
			}
		}
		else
		{
			if (!SetStdHandle(STD_INPUT_HANDLE, INVALID_HANDLE_VALUE))
			{
				Application::RequestExit(-1);
			}

			hConsole = CreateFileW(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
			if (hConsole != INVALID_HANDLE_VALUE)
			{
				SetConsoleOutputCP(CP_UTF8);

				SetConsoleCP(CP_UTF8);

				CONSOLE_CURSOR_INFO cf;
				GetConsoleCursorInfo(hConsole, &cf);
				cf.bVisible = false;

				SetConsoleCursorInfo(hConsole, &cf);

			
				if (!SetStdHandle(STD_OUTPUT_HANDLE, hConsole))
					Application::RequestExit(-1);


				//if (!SetConsoleTitleW(String::Format("%ls developer console (b%d)", *GetApplication()->GetApplicationName(), BUILD_NUMBER).Chr()))
				{
					//Application::RequestExit(-1);
				}

				bHasConsoleWindow = true;
			}
		}

		SetUnhandledExceptionFilter(ExceptionHandler);
	}
#endif // MR_DEBUG

	ILogger::Initialize();
}

void WindowsLogger::Shutdown()
{
#ifdef MR_DEBUG
	if (bIsRunningDebugMode && bHasConsoleWindow)
	{
		CloseHandle(hConsole);

		FreeConsole();
	}
#endif // MR_DEBUG

	ILogger::Shutdown();
}


void WindowsLogger::SendToOutputBuffer(const String* Buffer)
{
#ifdef MR_DEBUG
	if (bIsRunningDebugMode && bHasConsoleWindow)
	{
		const LogDescriptor* actualDescriptor = ILogger::Get()->GetActualEntry();
		if (!actualDescriptor)
			return;

		switch (actualDescriptor->severity)
		{
		case Log:
			SetConsoleTextAttribute(hConsole, 0x7);
			break;
		case Verbose:
			SetConsoleTextAttribute(hConsole, 0x9);
			break;
		case Error:
			SetConsoleTextAttribute(hConsole, 0xC);
			break;
		case Warn:
			SetConsoleTextAttribute(hConsole, 0x6);
			break;
		case Fatal:
			SetConsoleTextAttribute(hConsole, 0x4);
			break;
		}

		DWORD written = 0;
		//if (!WriteConsoleW(hConsole, **Buffer, (DWORD)strlen(**Buffer), &written, 0))
		//	return ILogger::SendToOutputBuffer(Buffer);

		//if (IsDebuggerAttached()) {}
			//OutputDebugStringW(**Buffer);

	}
#endif // MR_DEBUG

	ILogger::SendToOutputBuffer(Buffer);
}

void WindowsLogger::HandleFatal()
{
	const LogDescriptor* actualDescriptor = ILogger::Get()->GetActualEntry();
	if (!actualDescriptor)
	{
		MessageBoxW(nullptr, L"Unknown error!", L"Engine Error!", MB_OK | MB_ICONERROR);

		TerminateProcess(GetCurrentProcess(), -1);
		return;
	}

	//MessageBoxW(nullptr, actualDescriptor->message, L"Engine Error!", MB_OK | MB_ICONERROR);

	TerminateProcess(GetCurrentProcess(), -1);
}

void WindowsLogger::TransmitAssertion(const LogAssertion* Info)
{
	//MessageBoxW(nullptr, Info->assertMessage, L"Engine Error - Assertion failed!", MB_OK | MB_ICONERROR);

	TerminateProcess(GetCurrentProcess(), -1);
}

bool WindowsLogger::IsDebuggerAttached()
{
	return IsDebuggerPresent() ? true : false;
}