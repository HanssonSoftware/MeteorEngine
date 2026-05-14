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
				const String value = String(Processing::GetQuotedWord(input));

				inModule->commands[inCommandName].Add(value);

				if (!Processing::ExpectedCharacter(input, ','))
					break;

				input++; // ,
			}

			input++; // }
		}
	}
}


Module Module::MakeModuleFromBuffer(const char* buffer)
{
	Module instance;

	const StringView header = Processing::GetWord(buffer);
	if (!strncmp(header.ptr, "Module", header.size))
	{
		instance.moduleName = Processing::GetQuotedWord(buffer);

		if (Processing::ExpectedCharacter(buffer, ':'))
		{
			buffer++; // :

			instance.parent = Processing::GetQuotedWord(buffer);

			buffer++;
			if (Processing::ExpectedCharacter(buffer, '{'))
			{
				buffer++;

				const char* last = buffer;
				while (*buffer)
				{
					const String command = String(Processing::GetWord(buffer));
					
					//constexpr u64 b = "Dependencies"_h;

					switch (Hash(command))
					{
					case 5786862590791793456:  // IncludePath
					case 11138787046201537241: // IncludePaths
						Processing::EnterBlock(buffer, command, &instance);
						break;

					case 14686587794296974560: // Dependencies
						Processing::EnterBlock(buffer, command, &instance);
						break;

					default:
						Processing::SkipBlock(buffer);
						break;
					}

					if (last == buffer)
					{
						MR_LOG(LogProcessing, Error, "Parser stuck!");
						break;
					}
					else
					{
						last = buffer;
					}
				}
			}
		}
		else
		{
			MR_LOG(LogProcessing, Error, "Module has no parent! %s", *instance.moduleName);
		}
	}
	else
	{
		MR_LOG(LogProcessing, Fatal, "Failed to parse script header word! %s", header.ptr);
	}

	return instance;
}

	