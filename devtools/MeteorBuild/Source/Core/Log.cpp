/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Log.h"
#include "Application.h"
#include <Types/StringW.h>

static LONG ExceptionHandler(EXCEPTION_POINTERS* ptr)
{
	return 4;
}

static int WINAPI HRoutine(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
		GetApplication<BuildSystemApplication>()->Shutdown();
		break;
	case CTRL_LOGOFF_EVENT:
		break;
	case CTRL_SHUTDOWN_EVENT:
		break;
	}

	return 1;
}

BuildSystemLogger::BuildSystemLogger()
{
}

BuildSystemLogger::~BuildSystemLogger() noexcept
{
}

void BuildSystemLogger::Initialize()
{
	hConsole = CreateFileW(L"CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hConsole == INVALID_HANDLE_VALUE)
		Application::RequestExit(-1);

	SetConsoleOutputCP(CP_UTF8);

	SetConsoleCP(CP_UTF8);

	CONSOLE_CURSOR_INFO cf;
	GetConsoleCursorInfo(hConsole, &cf);
	cf.bVisible = false;

	SetConsoleCursorInfo(hConsole, &cf);

	if (!SetStdHandle(STD_OUTPUT_HANDLE, hConsole) &&
		!SetStdHandle(STD_INPUT_HANDLE, hConsole) &&
		!SetStdHandle(STD_ERROR_HANDLE, hConsole))
		Application::RequestExit(-1);

	SetUnhandledExceptionFilter(ExceptionHandler);

	SetConsoleCtrlHandler(HRoutine, 1);
	SetConsoleTitleW(L"Meteor Build(R)");
}

void BuildSystemLogger::Shutdown()
{
	CloseHandle(hConsole);
}

static StringW fullMessage;
void BuildSystemLogger::FormatLogMessage(LogDescriptor* Descriptor)
{
	
	
	//MultiByteToWideChar(CP_UTF8, 0, Descriptor->message, -1, nullptr, 0);

	//FormatConsoleColorBySeverity(Descriptor->severity);
	//SendToOutputBuffer(nullptr);
}

void BuildSystemLogger::TransmitAssertion(const LogAssertion* Info)
{

}

void BuildSystemLogger::SendToOutputBuffer(const String* Buffer)
{
	DWORD written = 0;

	if (!WriteConsoleW(hConsole, fullMessage, fullMessage.Length(), &written, nullptr))
	{

	}

	//FormatConsoleColorBySeverity(Log);
}

void BuildSystemLogger::HandleFatal(LogDescriptor* Descriptor)
{
}

static constexpr inline void FormatConsoleColorBySeverity(short Severity) noexcept
{
	switch (Severity)
	{
	case Log:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 37);
		break;
	case Warn:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 33);
		break;
	case Error:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 31);
		break;
	case Fatal:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 31);
		break;
	case Verbose:
		break;
	}
}
