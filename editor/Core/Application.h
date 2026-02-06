/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Application/Application.h>
#include <Types/Vector.h>
#include <WindowManager/WindowManager.h>
#include <Commandlet.h>
//#include <Types/Pointers.h>

LOG_ADDCATEGORY(EditorApplication);

class EditorApplication : public Application
{
public:
	EditorApplication();

	virtual void Init() override;

	virtual void Run() override;

	virtual void Shutdown() override;
}; 