/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

struct ConfigurationToolkit;

struct IDEConfiguration
{
	IDEConfiguration() = default;

	const char* configurationName = nullptr; // "Debug"

	ConfigurationToolkit* usedToolkit = nullptr;
};