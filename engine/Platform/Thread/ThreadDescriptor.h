/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once

struct ThreadInitializationDescriptor
{
	bool bStartWithPaused = false;

	void* functionPointer;
};