/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once

enum TimeEvaluationTarget
{
	TIME_EVALUATION_MICROSECONDS,
	TIME_EVALUATION_MILLISECONDS,
	TIME_EVALUATION_SECONDS
};

class PerformanceTimer
{
public:
	PerformanceTimer();

	~PerformanceTimer();

	void Start();

	void Stop();

	double Evaluate(const TimeEvaluationTarget defaultTarget = TIME_EVALUATION_MILLISECONDS);

	double GetDuration() const { return duration; }
private:
	bool bFired = false;

	unsigned long long start = 0;

	unsigned long long end = 0;

	double duration = 0;
};

