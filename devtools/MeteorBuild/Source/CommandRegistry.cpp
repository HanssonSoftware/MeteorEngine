/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "CommandRegistry.h"

LOG_ADDCATEGORY(CommandRegistry);

void CommandRegistry::RegisterCommand(const char* name, const char* descriptive, CommandInformationFunctionSignature func)
{
	if (count > MAX_COMMANDS)
		return;

	commands[count > 0 ? (count - 1) : 0] = { name, descriptive, func };
	count++;
}

bool CommandRegistry::CallFunctionOnCommand(const char* functionName) const
{
	for (u32 i = 0; i < count; i++)
	{
		if (strcmp(commands[i].command, functionName) == 0)
		{
			MR_LOG(LogCommandRegistry, Log, "Calling %s parameter\'s function!", functionName);

			commands->function();
			return true;
		}
	}

	return false;
}

