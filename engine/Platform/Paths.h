/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
class String;


struct IPaths
{
	static String GetSystemDirectory();

	static String GetDocumentsDirectory();

	static String GetUserDirectory();
	
	static String GetEngineDirectory();

	static String GetEngineSavedDirectory();

	static String GetSavedGameDirectory();

	static String GetExecutableDirctory();
};
