/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "BaseBuildMethod.h"
#include <Types/Array.h>
#include <Types/StringW.h>

class Project;
class Module;

class BuildProjectMethod : public BaseBuildMethod
{
public:
	BuildProjectMethod();

	virtual bool AcquireRequiredParameters() override;

	virtual void BeginCreating() override;

	virtual void Finalize() override;

	virtual void CleanUp() override;

protected:
	virtual ~BuildProjectMethod() noexcept = default;

	virtual Project* ParseProject(char* buffer);

	virtual Module* ParseModule(char* buffer);

	enum ScriptType
	{
		None,
		Module,
		Project
	};

	ScriptType DetectScriptType(const char* buffer, uint32_t length) const noexcept;

	void SetSpecifier(::Module* module, const char* verb, const char* verbEntry, uint32_t length) noexcept;

	StringW sourceDirectory;

	StringW intermediateDirectory;

	StringW alternativeSolutionDir;

	Array<StringW> foundScripts;

	Array<::Module*> modules;
};

