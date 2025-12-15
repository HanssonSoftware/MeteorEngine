/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <vector>

class IThread;

class IThreadPool
{
public:
	IThreadPool() = default;

	virtual ~IThreadPool() = default;

	virtual IThread* CreateThread() = 0;

	virtual void RemoveThread(const int32_t ID) = 0;

	virtual void SuspendThread(const int32_t ID) = 0;

	virtual void RemoveThread(const IThread* Specific) = 0;

	virtual void SuspendThread(const IThread* Specific) = 0;

	virtual void Shutdown() = 0;

protected:
	std::vector<IThread*> systemRequiredThreads;

	std::vector<IThread*> threads;
};

