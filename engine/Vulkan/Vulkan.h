/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Renderer/Renderer.h>
#include <Types/Array.h>

#include <vulkan/vulkan.h>

class Vulkan
{
public:
	virtual void StartupModule();

	virtual void ShutdownModule();

	virtual void Update();
protected:
	VkInstance instance;

	Array<const char*> enabledLayers;

	Array<const char*> enabledExtensions;
};


//IMPLEMENT_MODULE(Vulkan);