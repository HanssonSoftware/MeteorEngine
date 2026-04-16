/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Map.h>
#include <DataTypes.h>

struct Command
{
private:
	typedef void (*CallCommand)();
public:
	const char* word = nullptr;

	const char* description = nullptr;

	u8 priority = 0;

	CallCommand command = nullptr;

	constexpr Command(u8 priority, const char* name, const char* description, CallCommand fn)
		: priority(priority)
		, word(name)
		, description(description)
		, command(fn)
	{
		CommandsRegistry::GetCommandsRegistry().RegisterCommand(this);
	}

	~Command() noexcept
	{

	}
};


class CommandsRegistry
{
	static CommandsRegistry registry;

	static inline Map<const char*, Command*> commands;

	virtual ~CommandsRegistry() noexcept = default;
public:
	CommandsRegistry() = default;

	static inline CommandsRegistry& GetCommandsRegistry() { return registry; };

	static void RegisterCommand(Command* commandToAdd) { commands[commandToAdd->word] = commandToAdd; };

	static const Map<const char*, Command*>& GetCommand() { return commands; };
};


#define ADD_NEW_BUILD_COMMAND(Priority, Name, Desc, Func) \
	static Command Name##Command = { Priority, #Name, Desc, Func};

