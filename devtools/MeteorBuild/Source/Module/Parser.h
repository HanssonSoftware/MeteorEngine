/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

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


ECharacterType GetCharacterType(char*& str);

bool SkipCharacterType(char*& str, ECharacterType type);

String GetWord(char*& in, bool bStep = true);

bool SkipWord(char*& in, int& line, int& character);

bool IsSpace(const char* buffer);


