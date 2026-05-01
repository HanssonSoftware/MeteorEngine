/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Module.h"
#include <Logging/Log.h>
#include <Memory/MemoryBlockArena.h>

LOG_ADDCATEGORY(Processing);

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

				MemoryBlockArena<char> arrayOfParsedCommands = { 4_mB };

				const char* last = buffer;
				while (*buffer)
				{
					const String command = Processing::GetWord(buffer);
					
					constexpr u64 b = "IncludePath"_h;

					switch (Hash(command))
					{
					case 5786862590791793456:  // IncludePath
					case 11138787046201537241: // IncludePaths
						instance.commands[command] = 
						
						break;

					default:
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
			MR_LOG(LogProcessing, Error, "Module has no parent! (%s)", *instance.moduleName);
		}
	}
	else
	{
		MR_LOG(LogProcessing, Fatal, "Failed to parse script header word! %s", header.ptr);
	}

	return instance;
}
