/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Renderer.h"
#include <Module/ModuleManager.h>

void Renderer::StartupModule()
{
	int32_t j = 532;
}

void Renderer::ShutdownModule()
{
	int32_t j = 532;
}

void Renderer::Update()
{
}

Renderer* Renderer::InitModule(const String& name)
{
	ModuleManager::Get().LoadModule(String::Format("Renderer%ls", *name));


	return nullptr;
}
