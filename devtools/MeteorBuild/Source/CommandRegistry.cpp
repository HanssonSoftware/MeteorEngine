/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "CommandRegistry.h"
#include <cstring>

LOG_ADDCATEGORY(CommandRegistry);

constexpr void CommandRegistry::RegisterCommand(const char* name, const char* descriptive, CommandInformationFunctionSignature func)
{
	if (count > MAX_COMMANDS)
		return;

	commands[count > 0 ? count : 0] = { name, descriptive, func };
	count++;
}

bool CommandRegistry::CallFunctionOnCommand(const char* functionName) const
{
	for (u32 i = 0; i < count; i++)
	{
		if (strcmp(commands[i].command, functionName) == 0)
		{
			MR_LOG(LogCommandRegistry, Log, "Calling %hs parameter\'s function!", functionName);

			commands[i].function();
			return true;
		}
	}

	return false;
}

