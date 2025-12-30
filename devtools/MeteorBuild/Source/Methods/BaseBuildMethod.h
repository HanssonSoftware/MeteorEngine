/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/StringW.h>

class BaseBuildMethod
{
public:
	BaseBuildMethod() = default;

	virtual bool AcquireRequiredParameters() = 0;

	virtual void BeginCreating() = 0;
protected:
	virtual ~BaseBuildMethod() noexcept = default;

	StringW name;

	uint64_t start;
	uint64_t end;
};

