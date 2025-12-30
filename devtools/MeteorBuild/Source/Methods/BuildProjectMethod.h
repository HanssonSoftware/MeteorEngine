/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include "BaseBuildMethod.h"

class BuildProjectMethod : public BaseBuildMethod
{
public:
	BuildProjectMethod();

	virtual bool AcquireRequiredParameters() override;

	virtual void BeginCreating() override;

protected:
	virtual ~BuildProjectMethod() noexcept = default;

	StringW sourceDirectory;

	StringW intermediateDirectory;

};

