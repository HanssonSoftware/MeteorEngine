/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Logging/Log.h>

class BuildSystemLogger : public Logger
{
public:
	BuildSystemLogger();
	virtual ~BuildSystemLogger() noexcept;

	virtual void Initialize();
	virtual void Shutdown();

	virtual void TransmitMessage(LogDescriptor* Descriptor);
	virtual void TransmitAssertion(const LogAssertion* Info);
	virtual void SendToOutputBuffer(const String* Buffer);
	virtual void HandleFatal(LogDescriptor* Descriptor);
};

