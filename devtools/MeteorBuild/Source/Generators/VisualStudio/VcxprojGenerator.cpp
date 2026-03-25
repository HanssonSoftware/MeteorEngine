/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "VcxprojGenerator.h"
#include <Logging/Log.h>

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
