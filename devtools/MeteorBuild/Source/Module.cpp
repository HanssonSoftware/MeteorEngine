/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Module.h"
#include <Logging/Log.h>
#include <Memory/MemoryBlockArena.h>

LOG_ADDCATEGORY(Processing);

namespace Processing
{
	void EnterBlock(const char*& input, const char* inCommandName, Module* inModule)
	{
		if (Processing::ExpectedCharacter(input, ':'))
			input++;

		if (Processing::ExpectedCharacter(input, '{'))
		{
			input++; // {

			while (!Processing::ExpectedCharacter(input, '}'))
			{
				inModule->commands[inCommandName].Add(String(Processing::GetQuotedWord(input)));

				if (!Processing::ExpectedCharacter(input, ','))
					break;

				input++; // ,
			}

			input++; // }
		}
	}
}


Module::~Module() noexcept
{
}

Module::Module(const Module& old)
	: moduleName(old.moduleName)
	, parent(old.parent)
	, commands(old.commands)
	, files(old.files)
{

}

Module::Module(Module&& old) noexcept
{
	moduleName = std::move(old.moduleName);
	parent = std::move(old.parent);
	commands = std::move(old.commands);
	files = std::move(old.files);
}

Module& Module::operator=(const Module& old) noexcept
{
	moduleName = old.moduleName;
	parent = old.parent;
	commands = old.commands;
	files = old.files;
	return *this;
}

Module& Module::operator=(Module&& old) noexcept
{
	moduleName = std::move(old.moduleName);
	parent = std::move(old.parent);
	commands = std::move(old.commands);
	files = std::move(old.files);

	old.moduleName = "";
	old.parent = "";
	return *this;
}


	