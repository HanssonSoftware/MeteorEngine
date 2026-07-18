/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Application/Application.h>
#include <Logging/LogMacros.h> 

#include "BuildSystem.h"

LOG_ADDCATEGORY(BuildSystemApplication);

#define BUILDSYSTEM_API __declspec(dllexport)

struct BUILDSYSTEM_API BuildSystemApplication : public Application
{
public:
	BuildSystemApplication();

	virtual void Init() override;

	virtual void Run(float dt) override;

	virtual void Shutdown() override;

	BuildSystem& GetBuildSystem()
	{
		static BuildSystem instance;
		return instance;
	}
};



