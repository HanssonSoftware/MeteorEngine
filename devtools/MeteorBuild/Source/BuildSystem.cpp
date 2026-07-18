/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "BuildSystem.h"
#include <Logging/Log.h>
#include <HAL/Commandline.h>
#include <HAL/HAL.h>

#include <CommandRegistry.h>
#include <Application/Application.h>

LOG_ADDCATEGORY(CommandlineDispatcher);

void BuildSystem::CallRequiredMode()
{
	if (GetApplication()->GetCommandline()->GetInternal()[1] != nullptr)
	{
		CommandRegistry::Get().CallFunctionOnCommand(GetApplication()->GetCommandline()->GetInternal()[1]);
	}
	else
	{
		PrintHelp();
	}
}

void BuildSystem::PrintHelp()
{
	if (GetApplication()->GetCommandline()->GetInternal()[1] != nullptr)
	{

	}

	constexpr const char helpMsg[] = "No parameters in input!\n";
	//MR_LOG(LogTemp, Log, "%hs", helpMsg);
	
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
