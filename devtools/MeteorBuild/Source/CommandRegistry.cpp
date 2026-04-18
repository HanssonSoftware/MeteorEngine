/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "CommandRegistry.h"

void CommandRegistry::RegisterCommand(const char* name, const char* descriptive, CommandInformationFunctionSignature func)
{
	commands[count > 0 ? (count - 1) : 0] = { name, descriptive, func };
	count++;
} 
