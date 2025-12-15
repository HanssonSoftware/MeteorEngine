/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <stdint.h>
#include <Types/String.h>

struct Time
{
    uint32_t year = 0;
    uint32_t month = 0;
    uint32_t day = 0;
    uint32_t hour = 0;
    uint32_t minute = 0;
    uint32_t second = 0;
    uint32_t millisecond = 0;
};

class ITimer 
{
public:
    ITimer() = default;

    virtual ~ITimer() = default;

    virtual void Start() = 0;

    virtual void Stop() = 0;

    virtual void Reset() = 0;

    bool IsRunning() const { return bRunning; };

    static Time Now() noexcept;

    static String Format(const String* formatting);
protected:
    uint64_t begin = 0;

    uint64_t end = 0;

    bool bRunning = false;
};

typedef ITimer Timer;
