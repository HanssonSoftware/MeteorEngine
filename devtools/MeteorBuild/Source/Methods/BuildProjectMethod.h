/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include "BaseBuildMethod.h"
#include <Types/Array.h>
#include <Types/String.h>

#include <Memory/MemoryBlockArena.h>

class Project;
class Module;

class BuildProjectMethod : public BaseBuildMethod
{
public:
	BuildProjectMethod();

	bool AcquireRequiredParameters();

	virtual void StartMethod() override;

	virtual void CleanUp() override;

protected:
	virtual ~BuildProjectMethod() noexcept = default;

	virtual Module* ParseModule(wchar_t* buffer);

	virtual inline String GenerateGUID() const;

	String ConvertPath(String* wideBuffer);

	enum ScriptType
	{
		None,
		Module,
		Project
	};

	inline BuildProjectMethod::ScriptType DetectScriptType(const wchar_t* buffer, uint32_t length) const noexcept;

	inline void SetSpecifierForModule(::Module* module, const wchar_t* verb, const wchar_t* verbEntry, uint32_t length) noexcept;

	inline void SetSpecifierForProject(::Project* project, const wchar_t* verb, const wchar_t* verbEntry, uint32_t length) noexcept;

	String sourceDirectory;

	String intermediateDirectory;

	String alternativeSolutionDir;

	Array<String> foundScripts;

	Array<::Module*> modules;

	//MemoryBlockArena methodArena;
};

