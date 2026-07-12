/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Types/String.h>

#ifdef MR_CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif // MR_CORE_EXPORTS

namespace HAL
{
	namespace Paths
	{
		CORE_API String GetExecutablePath();

		CORE_API String GetEffectsPath();

		CORE_API char* NormalizePath(char* inPath);

		CORE_API char* DenormalizePath(char* inPath);
	};
}