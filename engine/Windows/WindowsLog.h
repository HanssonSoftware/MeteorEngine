/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Logging/Log.h>
#include <stdint.h>

class IFile;

class WindowsLogger : public ILogger
{
	friend class ILogger;
public:
	virtual ~WindowsLogger() noexcept;

	virtual void Initialize() override;

	virtual void Shutdown() override;

	virtual void HandleFatal() override;

	void TransmitAssertion(const LogAssertion* Info);

	virtual void SendToOutputBuffer(const String* Buffer) override;

	static inline bool IsDebuggerAttached();

protected:
	WindowsLogger();

	bool bHasConsoleWindow = false;

	IFile* consoleBuffer = nullptr;
};