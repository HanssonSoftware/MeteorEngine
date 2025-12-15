/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/Paths.h>
#undef GetSystemDirectory

struct WindowsPaths : public IPaths
{
	static String GetSystemDirectory();

	static String GetDocumentsDirectory();

	static String GetUserDirectory();

	static String GetEngineDirectory();

	static String GetEngineSavedDirectory();

	static String GetSavedGameDirectory();

	static String GetExecutableDirctory();

private:
	enum class EFolderGet { System, Documents, User, Engine, EngineSave, SavedGame };
};

using Paths = WindowsPaths;