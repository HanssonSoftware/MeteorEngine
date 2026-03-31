/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <DataTypes.h>

enum FileTypes
{
	FILE_TYPE_NONE,
	FILE_TYPE_SOURCE,
	FILE_TYPE_HEADER,
	FILE_TYPE_NATVIS,
	FILE_TYPE_SOLUTION,
	FILE_TYPE_LIBRARIES,
	FILE_TYPE_RESOURCES,
	FILE_TYPE_METEORBUILD_SCRIPT,
	FILE_TYPE_OTHER
};

struct ProjectFileEntries
{
	FileTypes type;
	const char* filePath;
};

inline constexpr const u8 GetFileTypeRanking(FileTypes type)
{
	switch (type)
	{
	case FILE_TYPE_NONE:
		return 128;
		
	case FILE_TYPE_SOURCE:
		return 80;
	case FILE_TYPE_HEADER:
		return 100;
	case FILE_TYPE_NATVIS:
		return 120;
	case FILE_TYPE_SOLUTION:
		
	case FILE_TYPE_LIBRARIES:
		return 128;
	case FILE_TYPE_RESOURCES:
		return 70;
	case FILE_TYPE_METEORBUILD_SCRIPT:
		return 0;
	case FILE_TYPE_OTHER:
		
	default:
		break;
	}


	return 128;
}