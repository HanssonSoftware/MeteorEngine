/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <Types/Array.h>

class Project;

class Module
{
public:
	Module() = default;

	virtual	~Module() noexcept = default;

	bool ConstructProjectFile(String* output);

	bool GenerateProxyHeader(String* output);

	bool GenerateBootstrapHeader(String* output);

	bool GetIsParsed() const { return bIsParsed; }

	void SetIsParsed(bool newVal) { bIsParsed = newVal; }

	String moduleName; // "Core"

	String defineName; // "CORE(_EXPORTS)"

	String generatedProjectFile; // ..\\Core.vcxproj

	String modulePath; // ..\\Core.mrbuild

	String dependsOn; // "Meteor"

	Array<String> files; // Detected sources

	Array<String> includePaths; // Includepaths

	Array<String> requires; // Dependency modules: Core, Platform, etc.

	String identification; // Guid

	Project* parent = nullptr;
protected:
	bool bIsParsed = false;
};

