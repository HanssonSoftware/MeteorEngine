/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/Winapi.h>

class BaseBuildMethod;

class BuildSystem
{
public:
	bool UseBuildSystem();

	template<typename T = BaseBuildMethod>
	T* GetCurrentMethod() const { return (T*)currentMethod; }

	//bool GenerateImportExportDefinitions(String* path);

	//Module* FindModule(const String* name);

	void Shutdown();
protected:
	bool ReadArguments();

	void SendHelpInfo() const;

	bool BuildProjectFiles();

	HANDLE hConsoleRef;

	BaseBuildMethod* currentMethod;
};

