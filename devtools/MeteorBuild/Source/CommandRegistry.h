/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Map.h>

typedef void (*CommandInformationFunctionSignature)();

struct CommandInformation
{
	const char* command = nullptr;
	const char* description = nullptr;

	CommandInformationFunctionSignature function = nullptr;
};


class CommandRegistry
{
public:
	static CommandRegistry& Get()
	{
		static CommandRegistry instance;
		return instance;
	}

	void RegisterCommand(const char* name, const char* descriptive, CommandInformationFunctionSignature func);
	bool CallFunctionOnCommand(const char* functionName) const;

	const CommandInformation& GetCommandsList() const { return *commands; };
	const u32 GetCommandsListCount() const { return count; };
protected:
	static constexpr const u32 MAX_COMMANDS = 32;
	CommandInformation commands[MAX_COMMANDS];

	u32 count = 0;
};

struct CommandMacroAuxiliary
{
	CommandMacroAuxiliary(const char* name, const char* descriptive, CommandInformationFunctionSignature func)
	{
		CommandRegistry::Get().RegisterCommand(name, descriptive, func);
	}
};

#define ADD_NEW_BUILD_COMMAND(Name, Description, Function) \
	static CommandMacroAuxiliary AuxiliaryStruct##Function(Name, Description, Function)
	