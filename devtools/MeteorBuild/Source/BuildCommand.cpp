/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Commands.h"
#include <Logging/Log.h>
#include "CommandRegistry.h"

namespace Commands
{
	ADD_NEW_BUILD_COMMAND("-build", "Builds your project", Build_Cmd);
	
	void Build_Cmd()
	{
		MR_LOG(LogTemp, Log, "BuildCMD called!");
	}
}