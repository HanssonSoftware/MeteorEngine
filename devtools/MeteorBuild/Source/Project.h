/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <Types/Array.h>

struct EngineModule;

class Project
{
	friend class BuildProjectMethod;
public:
	String projectName;

	Array<EngineModule*> modulesReference;

	Array<String> globalDefines;

	String launcher;

	String intermediatePath;
};

