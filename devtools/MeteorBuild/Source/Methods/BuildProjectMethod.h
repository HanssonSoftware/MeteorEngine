/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include "BaseBuildMethod.h"
#include <Types/Array.h>
#include <Types/StringW.h>

class BuildProjectMethod : public BaseBuildMethod
{
public:
	BuildProjectMethod();

	virtual bool AcquireRequiredParameters() override;

	virtual void BeginCreating() override;

	virtual void Finalize() override;
protected:
	virtual ~BuildProjectMethod() noexcept = default;

	StringW sourceDirectory;

	StringW intermediateDirectory;

	StringW alternativeSolutionDir;

	Array<StringW> foundScripts;

	//Array<> modules;
};

