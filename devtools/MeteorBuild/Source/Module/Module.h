/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <Types/Array.h>

class Project;

class Module
{
	friend class BuildProjectMethod;
public:
	Module() = default;

	virtual	~Module() noexcept = default;

protected:
	String moduleName; // "Core"

	String defineName; // "CORE(_EXPORTS)"

	String generatedProjectFile; // ..\\Core.vcxproj

	String modulePath; // ..\\Core.mrbuild

	String parent; // "Meteor"

	Array<String> files; // Detected sources

	Array<String> includePaths; // Includepaths

	Array<String> requires; // Dependency modules: Core, Platform, etc.

	char identification[38 + 1] = { '\0' }; // Guid
};

