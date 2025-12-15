/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "WindowsTimer.h"
#include "Windows.h"
#include <Logging/Log.h>
#include <Platform/Platform.h>

LOG_ADDCATEGORY(Timer);

WindowsTimer::WindowsTimer()
{
	timer = CreateWaitableTimerW(0, /*TRUE*/ 1, 0);
	if (timer == 0)
	{
		MR_LOG(LogTimer, Error, "CreateWaitableTimer failed!");
		return;
	}
}

void WindowsTimer::Start()
{
	bRunning = true;

	//SetWaitableTimer(timer, 0, 0, 0, 0, 0);
}

void WindowsTimer::Stop()
{
	bRunning = false;

	if (timer)
	{
		CloseHandle(timer);
		timer = 0;
	}
}

void WindowsTimer::Reset()
{
}

Time WindowsTimer::Now() noexcept
{
	SYSTEMTIME tm = {};
	GetLocalTime(&tm);

	Time newTime = {};
	newTime.year = tm.wYear;
	newTime.month = tm.wMonth;
	newTime.day = tm.wDay;
	newTime.hour = tm.wHour;
	newTime.minute = tm.wMinute;
	newTime.second = tm.wSecond;
	newTime.millisecond = tm.wMilliseconds;

	return newTime;
}

String WindowsTimer::Format(const String& formatting)
{
	wchar_t fixed[128] = {};
	//if (!GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, nullptr, formatting, fixed, 128, nullptr))
	{
		const String err = Platform::GetError();

		int j = 325;
	}


	if (!GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_FORCE24HOURFORMAT, nullptr, fixed, fixed, 128))
	{
		const String err = Platform::GetError();

		int j = 325;
	}

	return "fixed";
}

