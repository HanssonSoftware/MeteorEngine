/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <Types/Map.h>
#include <Types/Array.h>

namespace Processing
{
	inline void SkipSpaces(const char*& buffer)
	{
		while (isspace(*buffer))
			buffer++;
	}

	inline StringView GetWord(const char*& buffer)
	{
		SkipSpaces(buffer);

		u32 actualPos = 0;
		const char* begin = buffer;

		while (isalpha(*buffer))
		{
			buffer++; actualPos++;
		}

		return { begin, actualPos };
	}

	inline StringView GetQuotedWord(const char*& buffer)
	{
		SkipSpaces(buffer);

		u32 actualPos = 0;
		const char* begin = buffer;
		
		if (*buffer == '"')
		{
			buffer++; actualPos++; // "
			
			while (*buffer && *buffer != '"')
			{
				buffer++; actualPos++; // Word
			}

			buffer++; // "
		}
		else
		{
			return { begin, 0 };
		}

		return { begin + 1, actualPos - 1 };
	}

	inline bool ExpectedCharacter(const char*& in, const char what)
	{
		SkipSpaces(in);

		return strncmp(in, &what, 1) == 0;
	}
};


struct Module
{
	String moduleName;

	String parent;

	Map<const char*, Array<String>> commands;

	static Module MakeModuleFromBuffer(const char* buffer);
};
