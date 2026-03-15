/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>
#include <Platform/DataTypes.h>

class BaseBuildMethod
{
public:
	BaseBuildMethod() = default;

	virtual ~BaseBuildMethod() noexcept = default;

	virtual void StartMethod() = 0;

	virtual void CleanUp() = 0;

protected:
	String name;

	u64 start = 0;
	u64 end = 0;
};

