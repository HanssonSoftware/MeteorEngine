/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once

typedef void* HANDLE;
class String;

namespace VisualStudioTemplate
{
	void GeneratePropertySheetsBoilerplateCode(HANDLE fileHandle);

	void GenerateHeaderAndConfigurations(HANDLE fileHandle);

	void GenerateClosingBoilerplateCode(HANDLE fileHandle);

	void GenerateDynamicDetailsForIdentification(HANDLE fileHandle, const String* moduleName, const char* guid);

	void GenerateConfigurationRelatedCallsToCompiler(HANDLE fileHandle);
};
