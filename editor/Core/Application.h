/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Application.h>
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