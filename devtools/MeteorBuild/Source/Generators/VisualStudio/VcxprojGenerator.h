/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Generators/BaseGenerator.h>
#include <Platform/Winapi.h>

#include <Project/IDEConfig.h>

class VcxprojGenerator : public BaseGenerator
{
public:
	virtual bool Compile() override;

	void InsertClose(HANDLE file, FileTypes type);

	void WriteConfigurations() {};

	void WriteBoilerplate(HANDLE file, const Array<IDEConfiguration>* configs);
};

