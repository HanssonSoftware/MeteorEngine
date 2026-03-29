/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

typedef void* HANDLE;

struct VisualStudioStaticClass
{
	static void GeneratePropertySheetsBoilerplateCode(HANDLE fileHandle);

	static void GenerateFirstLinesOfBoilerplateCode(HANDLE fileHandle);

	static void GenerateClosingBoilerplateCode(HANDLE fileHandle);
};
