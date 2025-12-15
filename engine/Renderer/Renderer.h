/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Module/Module.h>

#include "Renderer.proxy.h"

class RENDERER_API Renderer : public Module
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	virtual void Update() override;

	static Renderer* InitModule(const String& name);
};


IMPLEMENT_MODULE(Renderer);