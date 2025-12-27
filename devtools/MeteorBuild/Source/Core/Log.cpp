/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Log.h"

BuildSystemLogger::BuildSystemLogger()
{
}

BuildSystemLogger::~BuildSystemLogger() noexcept
{
}

void BuildSystemLogger::Initialize()
{
}

void BuildSystemLogger::Shutdown()
{
}

void BuildSystemLogger::TransmitMessage(LogDescriptor* Descriptor)
{
}

void BuildSystemLogger::TransmitAssertion(const LogAssertion* Info)
{
}

void BuildSystemLogger::SendToOutputBuffer(const String* Buffer)
{
}

void BuildSystemLogger::HandleFatal(LogDescriptor* Descriptor)
{
}
