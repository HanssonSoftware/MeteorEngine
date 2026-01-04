/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/StringW.h>

class BaseBuildMethod
{
public:
	BaseBuildMethod() = default;

	virtual ~BaseBuildMethod() noexcept = default;

	virtual bool AcquireRequiredParameters() = 0;

	virtual void BeginCreating() = 0;

	virtual void Finalize() = 0;

	virtual void CleanUp() = 0;

protected:
	StringW name;

	uint64_t start = 0;
	uint64_t end = 0;
};

