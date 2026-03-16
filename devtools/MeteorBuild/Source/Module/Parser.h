/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>

class Module;
class Project;

namespace Parsing
{
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


	static ECharacterType GetType(char*& str);

	static bool SkipType(char*& str, ECharacterType type);

	static String GetWord(char*& in, bool bStep = true);

	static String GetQuotedWord(char*& in, bool bStep = true);

	static bool SkipWord(char*& in, int& line, int& character);

}