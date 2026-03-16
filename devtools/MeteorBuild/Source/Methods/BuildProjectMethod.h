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

	virtual Module* ParseModule(char* buffer);

	virtual inline String GenerateGUID() const;

	String ConvertPath(String* wideBuffer);

	enum ScriptType
	{
		None,
		Module,
		Project
	};

	inline BuildProjectMethod::ScriptType DetectScriptType(const char* buffer, u32 length) const noexcept;

	inline void SetSpecifierForModule(::Module* module, const char* verb, const char* verbEntry, u32 length) noexcept;

	inline void SetSpecifierForProject(::Project* project, const char* verb, const char* verbEntry, u32 length) noexcept;

	String sourceDirectory;

	String intermediateDirectory;

	String alternativeSolutionDir;

	Array<wchar_t*> foundScripts;

	Array<::Module*> modules;

	//MemoryBlockArena methodArena;
};

