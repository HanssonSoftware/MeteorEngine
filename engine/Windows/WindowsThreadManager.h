/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/Thread/ThreadManager.h>
#include <Platform/Thread/ThreadDescriptor.h>

class WindowsThread;

class WindowsThreadManager : public IThreadManager
{
public:
	static void Init();

	static void Shutdown();

	static WindowsThread* CreateThread(const ThreadInitializationDescriptor* descriptor);
};

using ThreadManager = WindowsThreadManager;