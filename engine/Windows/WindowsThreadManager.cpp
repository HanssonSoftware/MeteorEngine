/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "WindowsThreadManager.h"

#include <Windows.h>

DWORD inputFunction(LPVOID param)
{
	if (const ThreadInitializationDescriptor* convertedParam = (ThreadInitializationDescriptor*)&param)
	{
		
	}

	return 0;
}

void WindowsThreadManager::Init()
{

}

void WindowsThreadManager::Shutdown()
{

}

WindowsThread* WindowsThreadManager::CreateThread(const ThreadInitializationDescriptor* descriptor)
{
	if (!descriptor || !descriptor->functionPointer) 
		return nullptr;

	//HANDLE newHandle = ::CreateThread(
	//	nullptr,
	//	0,
	//	inputFunction,
	//	
	//	);

	return nullptr;
}
