/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once

#include "ThreadDescriptor.h"
#include <stdint.h>

class IThread
{
	IThread() = default;

	virtual void Init() = 0;

	virtual void Shutdown() = 0;

	virtual void Pause() = 0;

	virtual void Resume() = 0;

protected:
	bool bIsPaused = false;

	int32_t id = -1;
};

