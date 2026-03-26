/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "VcxprojGenerator.h"
#include <Logging/Log.h>
#include <Core/Utils.h>
#include <Types/Array.h>

LOG_ADDCATEGORY(VcxprojGen);

#define BITMASK(x, y) x << y

bool VcxprojGenerator::Compile()
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

void VcxprojGenerator::InsertClose(HANDLE file, FileTypes type)
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

void VcxprojGenerator::WriteBoilerplate(HANDLE file, const Array<IDEConfiguration>* configs)
{
	constexpr const char boilerplateConstant[] =
		"\t<Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />"
		"\t\t<ImportGroup Label=\"ExtensionSettings\">"
		"\t</ImportGroup>"
		"\t<ImportGroup Label=\"Shared\">"
		"\t</ImportGroup>";

	constexpr const u32 size = sizeof(boilerplateConstant) / sizeof(boilerplateConstant[0]);

	DWORD written = 0;
	if (!WriteFile(file, boilerplateConstant, size, &written, nullptr))
	{
		MR_LOG(LogVcxprojGen, Fatal, "Failed to write boilerplate code! (%d)  %s", ::GetLastError(), *Utils::GetError());
	}

	constexpr const char boilerplatePrintf[] =
		"\t<ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='%s|x64'\">" // here lies the (%s)|x64
		"\t\t<Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />"
		"\t</ImportGroup>";

	constexpr const u32 boilerplatePrintfSize = sizeof(boilerplatePrintf) / sizeof(boilerplatePrintf[0]);

	for (const IDEConfiguration& config : *configs)
	{
		if (config.configurationName)
		{
			char bigEnoughBuffer[512] = {};

			const u32 configNameSize = (u32)strlen(config.configurationName);

			snprintf(bigEnoughBuffer, boilerplatePrintfSize + configNameSize, boilerplatePrintf, config.configurationName);

			if (!WriteFile(file, boilerplateConstant, size, &written, nullptr))
			{
				MR_LOG(LogVcxprojGen, Fatal, "Failed to write boilerplate code! (%d)  %s", ::GetLastError(), *Utils::GetError());
				break;
			}
			
			continue;
		}
			
		MR_LOG(LogVcxprojGen, Fatal, "Invalid config name!");
	}
}
