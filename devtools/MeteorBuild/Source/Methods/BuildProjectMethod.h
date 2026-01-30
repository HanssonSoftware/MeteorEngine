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

	virtual Module* ParseModule(char* buffer);

	virtual inline String GenerateGUID() const;

	String ConvertPath(StringW* wideBuffer);

	enum ScriptType
	{
		None,
		Module,
		Project
	};

	inline BuildProjectMethod::ScriptType DetectScriptType(const char* buffer, uint32_t length) const noexcept;

	inline void SetSpecifierForModule(::Module* module, const char* verb, const char* verbEntry, uint32_t length) noexcept;

	inline void SetSpecifierForProject(::Project* project, const char* verb, const char* verbEntry, uint32_t length) noexcept;

	StringW sourceDirectory;

	StringW intermediateDirectory;

	StringW alternativeSolutionDir;

	Array<StringW> foundScripts;

	Array<::Module*> modules;
};

