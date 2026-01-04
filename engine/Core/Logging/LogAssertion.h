/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <stdint.h>

struct LogAssertion
{
	friend class Logger;

	LogAssertion() = delete;

	virtual ~LogAssertion() noexcept = default;

	LogAssertion(const char* inputFile, const int32_t inputLine, const char* statement, const char* inputMessage, ...);

	const char* assertLocationInFile;

	const char* assertStatement;

	String assertMessage;

	int32_t assertLineInFile = 0;

	int32_t bIgnoreFor = 0;

	bool bIgnoreThis = false;
};

