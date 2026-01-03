/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Delegate.h>

#ifdef MR_DEBUG
constexpr inline const bool bIsRunningDebugMode = true;
#else
constexpr inline const bool bIsRunningDebugMode = false;
#endif // MR_DEBUG

class String;
struct LogDescriptor;
struct LogAssertion;

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

CREATE_DELEGATE(LoggerInitialize);

struct LogEntry {};

class CORE_API Logger
{
public:
	static Logger* Get()
	{
		return instance ? instance : new Logger;
	};

	Logger() = default;
	Logger(Logger* newInstance);
	virtual ~Logger() noexcept;

	virtual void Initialize();
	virtual void Shutdown();

	virtual void TransmitMessage(LogDescriptor* Descriptor);
	virtual void TransmitAssertion(const LogAssertion* Info);
	virtual void SendToOutputBuffer(const String* Buffer);
	virtual void HandleFatal(LogDescriptor* Descriptor);

protected:
	LogDescriptor* current = nullptr;

	//DelLoggerInitialize loggerInitialized;

	bool bIsInitialized = false;

private:
	static inline Logger* instance;
};

#include "LogMacros.h"

static constexpr inline const char* FormatSeverity(LogSeverity Severity) noexcept
{
	switch (Severity)
	{
	case Log:
		return "Log";
	case Warn:
		return "Warning";
	case Error:
		return "Error";
	case Fatal:
		return "Fatal";
	case Verbose:
		return "Verbose";
	}

	return "???";
}

