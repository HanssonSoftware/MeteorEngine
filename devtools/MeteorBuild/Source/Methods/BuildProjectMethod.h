/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include "BaseBuildMethod.h"

class BuildProjectMethod : public BaseBuildMethod
{
public:
	BuildProjectMethod() = default;

	void GatherRequiredParameters();

protected:
	virtual ~BuildProjectMethod() noexcept = default;
};

