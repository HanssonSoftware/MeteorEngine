/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>

class Module;
class Project;

enum Ewchar_tacterType
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


Ewchar_tacterType GetType(wchar_t*& str);

bool SkipType(wchar_t*& str, Ewchar_tacterType type);

String GetWord(wchar_t*& in, bool bStep = true);

String GetQuotedWord(wchar_t*& in, bool bStep = true);

bool SkipWord(wchar_t*& in, int& line, int& wchar_tacter);

bool IsSpace(const wchar_t* buffer);


