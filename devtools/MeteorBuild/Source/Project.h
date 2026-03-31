/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <Types/Array.h>

class ScriptModule;

class Project
{
	friend class BuildProjectMethod;
public:
	String projectName;

	Array<ScriptModule*> modulesReference;

	Array<String> globalDefines;

	String launcher;

	String intermediatePath;
};

