/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "BuildSystem.h"
#include <Logging/Log.h>
#include <CommandRegistry.h>

void BuildSystem::CallRequiredMode()
{
}

void BuildSystem::PrintHelp()
{
	constexpr const char helpMsg[] = "No parameters in input!\n";
	MR_LOG(LogTemp, Log, "%s", helpMsg);
	
	const CommandInformation* commands = &CommandRegistry::Get().GetCommandsList();
	for (int i = 0; i < CommandRegistry::Get().GetCommandsListCount(); i++)
	{
		const CommandInformation* commandInfo = &commands[i];

		swprintf()
	}

#ifdef MR_PLATFORM_WINDOWS


#else

#endif // MR_PLATFORM_WINDOWS
}
