/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#ifdef MR_PLATFORM_WINDOWS
#include "Paths.h"
#include <HAL/HAL.h>

#include <Win32/MinimalWin.h>

namespace HAL
{
    namespace Paths
    {
        String GetExecutablePath()
        {
            wchar_t fixed[512] = {};
            char final[512] = {};
            HAL::ConvertToNarrow(final, GetModuleFileNameW(nullptr, fixed, 512), fixed);
                
            return final;
        }

        String GetEffectsPath()
        {
            return "";
        }

        char* NormalizePath(char* inString)
        {
            for (char* begin = inString; *begin; begin++)
                if (*begin == '\\') *begin = '/';

            return inString;
        }

        char* DenormalizePath(char* inString)
        {
            for (char* begin = inString; *begin; begin++)
                if (*begin == '/') *begin = '\\';

            return inString;
        }
    }
}

#endif