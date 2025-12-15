/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "LogAssertion.h"
#include <cstdarg>
#include <stdio.h>

LogAssertion::LogAssertion(const char* inputFile, const int32_t inputLine, const char* statement, const char* inputMessage, ...)
    : assertLocationInFile(inputFile)
    , assertLineInFile(inputLine)
    , assertStatement(statement)
{
    if (inputMessage)
    {
        va_list a = 0;
        va_start(a, inputMessage);
        const int32_t requiredAmount = vsnprintf(nullptr, 0, inputMessage, a);

        if (requiredAmount == 0)
        {
            va_end(a);
            assertMessage = "";
            return;
        }

        char* super = new char[requiredAmount + 1];
        vsnprintf(super, requiredAmount, inputMessage, a);
        va_end(a);

        assertMessage = super;
        delete[] super;
    }
}