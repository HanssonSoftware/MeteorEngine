/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "BuildSystem.h"
#include <Logging/Log.h>
#include <Commandlet.h>
#include <Platform/Platform.h>

#include <CommandRegistry.h>

void BuildSystem::CallRequiredMode()
{
	if (Commandlet::Get().GetArgs()[1] != nullptr)
	{
		char commandBuffer[128] = {};
		Platform::ConvertToNarrow(commandBuffer, wcslen(Commandlet::Get().GetArgs()[1]), Commandlet::Get().GetArgs()[1]);

		CommandRegistry::Get().CallFunctionOnCommand(commandBuffer);
	}
	else
	{
		PrintHelp();
	}
}

void BuildSystem::PrintHelp()
{
	if (Commandlet::Get().GetCount() < 1)
	{

	}

	constexpr const char helpMsg[] = "No parameters in input!\n";
	MR_LOG(LogTemp, Log, "%s", helpMsg);
	
	const CommandInformation* commands = &CommandRegistry::Get().GetCommandsList();
	for (int i = 0; i < CommandRegistry::Get().GetCommandsListCount(); i++)
	{
		const CommandInformation* commandInfo = &commands[i];
		//swprintf();
		
	}

#ifdef MR_PLATFORM_WINDOWS


#else

#endif // MR_PLATFORM_WINDOWS
}
