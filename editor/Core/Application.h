/* Copyright 2020 - 2026, Hansson Software. All rights reserved. */

#pragma once
#include <Application/Application.h>
#include <Types/Vector.h>

//#include <Types/Pointers.h>

LOG_ADDCATEGORY(EditorApplication);

class Window;

class EditorApplication : public Application
{
public:
	EditorApplication();

	virtual void Init() override;

	virtual void Run(float dt) override;

	virtual void Shutdown() override;

protected:
	Window* editorWindow;
}; 