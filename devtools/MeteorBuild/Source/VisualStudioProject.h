/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

typedef void* HANDLE;
class String;

struct VisualStudioStaticClass
{
	static void GeneratePropertySheetsBoilerplateCode(HANDLE fileHandle);

	static void GenerateFirstLinesOfBoilerplateCode(HANDLE fileHandle);

	static void GenerateClosingBoilerplateCode(HANDLE fileHandle);

	static void GenerateDynamicDetailsForIdentification(HANDLE fileHandle, const String* moduleName, const char* guid);
	
	static void GenerateConfigurationDetails(HANDLE fileHandle);
};
