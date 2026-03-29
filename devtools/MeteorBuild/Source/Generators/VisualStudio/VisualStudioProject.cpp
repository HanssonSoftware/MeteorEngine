/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "VisualStudioProject.h"
#include <Logging/Log.h>
#include <Core/Utils.h>
#include <Types/Array.h>

LOG_ADDCATEGORY(VcxprojGen);

#define BITMASK(x, y) x << y

bool VisualStudioProject::Compile()
{
	u32 bitWiseConditions = 0;

	for (ProjectFileEntries& entry : includedEntries)
	{
		switch (entry.type)
		{
		case FILE_TYPE_NONE:
			MR_LOG(LogVcxprojGen, Fatal, "Entries cannot be None! (%s)", entry.filePath);
			break;
		case FILE_TYPE_SOURCE:
			break;
		case FILE_TYPE_HEADER:
			break;
		case FILE_TYPE_NATVIS:
			break;
		case FILE_TYPE_SOLUTION:
			break;
		case FILE_TYPE_METEORBUILD_SCRIPT:
			break;
		case FILE_TYPE_OTHER:
			break;

		default:
			break;
		}
	}

	return false;
}

void VisualStudioProject::InsertClose(HANDLE file, FileTypes type)
{
	switch (type)
	{
	case FILE_TYPE_NONE:
		MR_LOG(LogVcxprojGen, Fatal, "Entries cannot be none!");
		break;
	case FILE_TYPE_SOURCE:
		break;
	case FILE_TYPE_HEADER:
		break;
	case FILE_TYPE_NATVIS:
		break;
	case FILE_TYPE_SOLUTION:
		break;
	case FILE_TYPE_METEORBUILD_SCRIPT:
		break;
	case FILE_TYPE_OTHER:
		break;

	default:
		break;
	}
}

void VisualStudioProject::WriteProjectConfigurationsBoilerplate(HANDLE file)
{
	constexpr const char boilerplateConfigurationTypes[] =
		"\t<Project DefaultTargets=\"Build\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
		"\t<ItemGroup Label=\"ProjectConfigurations\">\n"
		"\t<ProjectConfiguration Include=\"Debug|x64\">\n"
		"\t\t<Configuration>Debug</Configuration>\n"
		"\t\t<Platform>x64</Platform>\n"
		"\t</ProjectConfiguration>\n"
		"\t<ProjectConfiguration Include=\"Test|x64\">\n"
		"\t\t<Configuration>Test</Configuration>\n"
		"\t\t<Platform>x64</Platform>\n"
		"\t</ProjectConfiguration>\n"
		"\t<ProjectConfiguration Include=\"Shipping|x64\">\n"
		"\t\t<Configuration>Shipping</Configuration>\n"
		"\t\t<Platform>x64</Platform>\n"
		"\t</ProjectConfiguration>\n"
		"\t</ItemGroup>\n";
	
	constexpr const u32 boilerplateConfigurationTypesSize = sizeof(boilerplateConfigurationTypes) / sizeof(boilerplateConfigurationTypes[0]);

	DWORD written = 0;
	if (!WriteFile(file, boilerplateConfigurationTypes, boilerplateConfigurationTypesSize, &written, nullptr))
	{
		MR_LOG(LogVcxprojGen, Fatal, "Failed to write boilerplate code! (%d)  %s", ::GetLastError(), *Utils::GetError());
	}
}

void VisualStudioProject::WritePropertyPropsBoilerplate(HANDLE file)
{
	constexpr const char boilerplatePropertyProps[] =
		"\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\n"
		"\t\t<ImportGroup Label=\"ExtensionSettings\">\n"
		"\t</ImportGroup>\n"
		"\t<ImportGroup Label=\"Shared\">\n"
		"\t</ImportGroup>\n"
		"\t<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Debug|x64'\">\n"
		"\t\t<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\n"
		"\t</ImportGroup>\n"
		"\t<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Test|x64'\">\n"
		"\t\t<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\n"
		"\t</ImportGroup>\n"
		"\t<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='Shipping|x64'\">\n"
		"\t\t<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\n"
		"\t</ImportGroup>\n";

	constexpr const u32 boilerplatePropertyPropsSize = sizeof(boilerplatePropertyProps) / sizeof(boilerplatePropertyProps[0]);

	DWORD written = 0;
	if (!WriteFile(file, boilerplatePropertyProps, boilerplatePropertyPropsSize, &written, nullptr))
	{
		MR_LOG(LogVcxprojGen, Fatal, "Failed to write boilerplate code! (%d)  %s", ::GetLastError(), *Utils::GetError());
	}
}
