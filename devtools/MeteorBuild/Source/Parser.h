/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>

struct EngineModule;
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


	ECharacterType GetType(char*& str);

	bool SkipType(char*& str, ECharacterType type);

	String GetWord(char*& in, bool bStep = true);

	String GetQuotedWord(char*& in, bool bStep);

	bool SkipWord(char*& in, int& line, int& character);
}