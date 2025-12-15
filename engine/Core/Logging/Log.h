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
class IFile;

CREATE_DELEGATE(LoggerInitialize);

struct LogEntry {};

class Logger
{
public:
	static Logger* Get();

	Logger();

	virtual ~Logger() noexcept;

	virtual void Initialize();

	virtual void Shutdown();

	virtual void TransmitMessage(LogDescriptor* Descriptor);

	virtual void TransmitAssertion(const LogAssertion* Info);

	virtual void SendToOutputBuffer(const String* Buffer);

	virtual void HandleFatal(LogDescriptor* Descriptor);

protected:
	LogDescriptor* current;

	//DelLoggerInitialize loggerInitialized;

	bool bIsInitialized = false;
};

#include "LogMacros.h"
