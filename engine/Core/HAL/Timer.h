/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <HAL/DataTypes.h>

#ifdef MR_CORE_EXPORT
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

struct CORE_API Time
{
	Time(u16 years, u8 months, u8 days,u8 hours, u8 minutes,u8 seconds,u16 mills)
		: year(years), month(months), day(days), hour(hours), minute(minutes), second(seconds), mill(mills)
	{

	}

	~Time() noexcept = default;
	Time(Time&&) = delete;
	Time& operator=(Time&&) = delete;
	Time& operator=(const Time& other)
	{
		year = other.year;
		month = other.month;
		day = other.day;
		hour = other.hour;
		minute = other.minute;
		second = other.second;
		mill = other.mill;

		return *this;
	}

	Time(const Time& other)
	{
		year = other.year;
		month = other.month;
		day = other.day;
		hour = other.hour;
		minute = other.minute;
		second = other.second;
		mill = other.mill;
	}

	u16 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	u16 mill;
};

class CORE_API Timer
{

public:
	enum class CalculateBy
	{
		Milliseconds,
		Seconds,
		Minutes,
		Hours,
		Days
	};

	Time Now();

	void Start();

	void Stop();

	double Count(CalculateBy by);
protected:
	u64 start;
	u64 stop;
};

