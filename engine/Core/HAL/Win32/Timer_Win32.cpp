/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include "Timer.h"

#include <Win32/MinimalWin.h>

Time Timer::Now()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	return { (u16)st.wYear, (u8)st.wMonth, (u8)st.wDay, (u8)st.wHour, (u8)st.wMinute, (u8)st.wSecond, (u8)st.wMilliseconds };
}

void Timer::Start()
{
	LARGE_INTEGER lg;
	QueryPerformanceCounter(&lg);

	start = (u64)lg.QuadPart;
}

void Timer::Stop()
{
	LARGE_INTEGER lg;
	QueryPerformanceCounter(&lg);

	stop = (u64)lg.QuadPart;
}

double Timer::Count(CalculateBy by)
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	switch (by)
	{
	case Timer::Milliseconds:
		break;
	case Timer::Seconds:
		break;
	case Timer::Minutes:
		break;
	case Timer::Hours:
		break;
	case Timer::Days:
		break;
	default:
		break;
	}

	return ((double)stop - (double)start) / (double)freq.QuadPart;
}

#endif // MR_PLATFORM_WINDOWS

