/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#include "Application.h"
#include <Types/String.h>
#include <Memory/MemoryHandler.h>

#include <Special/LaunchMeteorSuite.h>
#include <Platform/Platform.h>

#include <shellapi.h>
//#pragma comment(lib, "Shell32.lib")


BuildSystemApplication::BuildSystemApplication()
	: Application()
{
	appName = "Meteor Build";
	appNameNoSpaces = "MeteorBuild";
	appCodeName = "Apollo";
}

void BuildSystemApplication::Init()
{
	Application::Init();

	GetBuildSystem().CallRequiredMode();
}

void BuildSystemApplication::Run()
{


	Application::RequestExit(0);
}

void BuildSystemApplication::Shutdown()
{
	SetAppState(ECurrentApplicationState::SHUTDOWN);
	Application::Shutdown();
}

int wmain(int argc, wchar_t argv[])
{  
#ifdef MR_PLATFORM_WINDOWS
	wchar_t path[512] = {};                                                                                                          
	DWORD count = GetModuleFileNameW(GetModuleHandleW(nullptr), path, 512);
	if (FAILED(PathCchRemoveFileSpec(path, count)))                                                                               
	{                                                                                                                                
		MessageBoxW(nullptr, L"Unable to canonicalize engine path!", L"Engine Error!", MB_OK);                                       
		return -1;                                                                                                                   
	}

	AddDllDirectory(path);                                                                                                           
	HMODULE entryPoint = LoadLibraryExW(L"MeteorEngine-Core.dll", nullptr, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS);
	if (entryPoint != INVALID_HANDLE_VALUE)                                                                                          
	{                                                                                                                                
		typedef int (*ProxyFunction)(Application*, int, wchar_t*);                                                                                  
		ProxyFunction externalLinkageFunction = (ProxyFunction)GetProcAddress(entryPoint, "LaunchApplication");                      
		if (externalLinkageFunction)                                                                                                 
		{                                                                                                                            
			BuildSystemApplication* application = new BuildSystemApplication;

			int Result = externalLinkageFunction(application, argc, argv);                                                                       

			if (!FreeLibrary(entryPoint))                                                                                            
				return -1;                                                                                                           

			delete application;                                                                                                      
			return Result;                                                                                                           
		}                                                                                                                            
	}                                                                                                                                

	MessageBoxW(nullptr, L"Unable to reach engine code!", L"Engine Error!", MB_OK);
#else
#error int main() platform dependant launch missing!
#endif // MR_PLATFORM_WINDOWS
	return -1;                                                                                                                       
};