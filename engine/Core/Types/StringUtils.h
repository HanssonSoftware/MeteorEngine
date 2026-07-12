/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <HAL/DataTypes.h>


constexpr u32 StringLength(const char* inString)
{
	if (!inString) return 0;

	const char* start = inString;

	while (*inString)
		inString++;

	return (u32)(inString - start);
}

static constexpr void StringCopy(const char* toCopy, char*& targetString)
{
	if (!toCopy || !targetString) return;

	u32 index = 0;
	while (toCopy[index] != '\0')
	{
		targetString[index] = toCopy[index];
		index++;
	}

	targetString[index] = '\0';
}

static constexpr void StringCopyN(const char* toCopy, char*& targetString, u32 N)
{
	if (!toCopy || !targetString, N > 0) return;

	u32 index = 0;
	while (index < N && toCopy[index] != '\0')
	{
		targetString[index] = toCopy[index];
		index++;
	}

	targetString[index] = '\0';
}