/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Module/Module.h>
#include <vulkan/vulkan.h>

class MemoryBlockPool;

class Vulkan : public Module
{
public:
	Vulkan() = default;
	virtual ~Vulkan() noexcept = default;

	virtual bool StartupModule();

	virtual void ShutdownModule();

	virtual void Update();
protected:

	struct VkFoundDevice
	{
		VkPhysicalDevice device;
		u32 queueFamilyIndex;
	};

	VkFoundDevice SelectBestCardForProject(VkPhysicalDevice* devicesList, u32 count);

	MemoryBlockPool* rendererMemoryPool;

	VkInstance instance;

	VkPhysicalDevice physicalDevice;

	VkSurfaceKHR surface;

	VkSwapchainKHR swapChain;

	VkDevice device;

	VkQueue graphQueue;
	VkQueue presQueue;

	u32 maxImages = 0;
	Array<VkImage> images;
	Array<VkImageView> imageViews;

	Array<const char*> enabledLayers;

	Array<const char*> enabledInstanceExtensions;

	Array<const char*> enabledExtensions;
};

//
IMPLEMENT_MODULE(Vulkan);