/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>

class Module;
class Project;

enum ECharacterType
{
	None,
	Word,
	Colon,
	SemiColon,
	Comma,
	OpenBrace,
	ClosedBrace,
	EndOfFile
};

struct FoundScriptData
{
	String name;

	String path;

	String full;
};

struct Parser
{
	static Module* ParseModuleScript(String* moduleName);

	static Project* ParseProjectScript(String* projectPath);

	static ECharacterType GetCharacterType(const char*& str);

	static void SkipCharacterType(const char*& str, ECharacterType type);

	static String GetWord(const char*& in, bool bStep);

	static void SkipWord(const char*& in);

	static bool IsSpace(const char* buffer);
};

