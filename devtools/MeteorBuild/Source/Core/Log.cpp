/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

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

void BuildSystemLogger::TransmitMessage(LogDescriptor* Descriptor)
{
	
}

void BuildSystemLogger::TransmitAssertion(const LogAssertion* Info)
{

}

void BuildSystemLogger::SendToOutputBuffer(const String* Buffer)
{
	wchar_t* super = MemoryManager::Get().Allocate<wchar_t>(Buffer->Length() + 1);
	if (!MultiByteToWideChar(CP_UTF8, 0, **Buffer, Buffer->Length() * sizeof(char), super, Buffer->Length()))
	{


		MemoryManager::Get().Deallocate(super);
	}

	DWORD written = 0;
	if (!WriteConsoleW(hConsole, super, Buffer->Length(), &written, nullptr))
	{

	}

	MemoryManager::Get().Deallocate(super);
}

void BuildSystemLogger::HandleFatal(LogDescriptor* Descriptor)
{
}
