/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "PerformanceTimer.h"
#include <Windows.h>

LARGE_INTEGER lg;
#include <Logging/Log.h>

LOG_ADDCATEGORY(HighPerformanceTimer);

static unsigned long long frequency = 0;
PerformanceTimer::PerformanceTimer()
{
#ifdef _WIN64
	/* On systems that run Windows XP or later, the function will always succeed and will thus never return zero. */
	QueryPerformanceFrequency(&lg);
	frequency = lg.QuadPart;
#endif // _WIN64
}

PerformanceTimer::~PerformanceTimer()
{
	if (bFired)
	{
		bFired = false;

#ifdef _WIN64

#endif // _WIN64
	}
}

void PerformanceTimer::Start()
{
	bFired = true;

#ifdef _WIN64
	LARGE_INTEGER lg;
	QueryPerformanceCounter(&lg);

	start = lg.QuadPart;
#endif // _WIN64
}

void PerformanceTimer::Stop()
{
	bFired = false;

#ifdef _WIN64
	LARGE_INTEGER lg;
	QueryPerformanceCounter(&lg);

	end = lg.QuadPart;
#endif // _WIN64

	duration = Evaluate();
}


double PerformanceTimer::Evaluate(const TimeEvaluationTarget defaultTarget)
{
	double calculated = 0.0;
	if (bFired == true)
		Stop();

	calculated = (end - start) / (double)frequency;

	switch (defaultTarget)
	{
	case TIME_EVALUATION_MICROSECONDS:
		return calculated * 1000000.0;
	case TIME_EVALUATION_MILLISECONDS:
		return calculated * 1000.0;
	case TIME_EVALUATION_SECONDS:
		break;
	}

	return calculated;
}
