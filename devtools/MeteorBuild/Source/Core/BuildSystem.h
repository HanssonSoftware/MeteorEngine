/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Types/String.h>
#include <Module/Module.h>

#include <Platform/File.h>

class Project;

class BuildSystem
{
protected:
	enum ERequestedCommandToDo
	{
		Build,
		Rebuild

	} command;
public:
	bool InitFramework();

	void OrderModules();

	bool BuildProjectFiles();

	const Array<Module>* GetModules() const { return &loadedModules; };

	Project& GetProjectScript() const { return *ps; }

	bool GenerateImportExportDefinitions(String* path);

	Module* FindModule(const String* name);
protected:
	bool ReadArguments();

	const ERequestedCommandToDo GetBuildCommand() const { return command; }

	Project* ps;

	Array<Module> loadedModules;
};

