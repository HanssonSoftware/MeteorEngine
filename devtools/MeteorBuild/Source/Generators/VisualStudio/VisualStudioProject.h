/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Generators/BaseProject.h>
#include <Platform/Winapi.h>


class VisualStudioProject : public BaseProject
{
public:
	virtual bool Compile() override;

	void InsertClose(HANDLE file, FileTypes type);

	void WriteConfigurations() {};

	void WriteProjectConfigurationsBoilerplate(HANDLE file);

	void WritePropertyPropsBoilerplate(HANDLE file);
};

