/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/Winapi.h>

class BaseBuildMethod;



class BuildSystem
{
public:
	bool UseBuildSystem();

	void Shutdown();
protected:
	bool ReadArguments();

	void SendHelpInfo() const;

	bool BuildProjectFiles();

	HANDLE hConsoleRef;
};

