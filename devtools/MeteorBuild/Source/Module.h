/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <Types/Map.h>
#include <Types/Array.h>

struct Module;

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

	inline void SkipBlock(const char*& toSkip)
	{
		SkipSpaces(toSkip);

		if (Processing::ExpectedCharacter(toSkip, ':'))
			toSkip++;

		if (Processing::ExpectedCharacter(toSkip, '{'))
		{
			toSkip++;

			while (Processing::ExpectedCharacter(toSkip, '}'))
				toSkip++;
		}
	}

	void EnterBlock(const char*& input, const char* inCommandName, Module* inModule);
};



struct Module
{
	Module() = default;
	~Module() noexcept = default;
	Module(const Module& old)
		: moduleName(old.moduleName)
		, parent(old.parent)
		, commands(old.commands)
	{

	}
	Module(Module&& old) noexcept
	{
		moduleName = std::move(old.moduleName);
		parent = std::move(old.parent);
		commands = std::move(old.commands);
	}

	Module& operator=(const Module& old) noexcept
	{
		moduleName = old.moduleName;
		parent = old.parent;
		commands = old.commands;
		return *this;
	};

	Module& operator=(Module&& old) noexcept
	{
		moduleName = old.moduleName;
		parent = old.parent;
		commands = old.commands;

		old.moduleName = "";
		old.parent = "";
		return *this;
	}

	String moduleName;

	String parent;

	String path;
#ifdef MR_PLATFORM_WINDOWS
	Array<wchar_t*> files;
#else
	Array<char*> fileHashes;
#endif // MR_PLATFORM_WINDOWS
	Map<const char*, Array<String>> commands;

	static Module MakeModuleFromBuffer(const char* buffer);
};
