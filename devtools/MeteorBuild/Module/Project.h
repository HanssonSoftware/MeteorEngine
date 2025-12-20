/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <Types/Array.h>
#include <Module/Module.h>

class Project
{
public:
	String projectName;

	bool Finalize(String* output);

	Array<Module>* modulesReference;

	Array<String> globalDefines;

	String launcher;

	String intermediatePath;
};

