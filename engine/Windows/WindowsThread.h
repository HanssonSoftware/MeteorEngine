/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Platform/Thread/Thread.h>

class WindowsThread : public IThread
{
public:
	virtual void Init();

	virtual void Shutdown();

	virtual void Pause() override;

	virtual void Resume() override;

	operator void*()
	{
		return handle;
	}

protected:
	void* /*HANDLE*/ handle;
};

