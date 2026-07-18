/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#pragma once
#include <Types/Array.h>
#include <Module/Module.h>
#include <vulkan/vulkan.h>

class MemoryBlockPool;

#ifdef MR_VULKAN_EXPORTS
#define VULKAN_API __declspec(dllexport)
#else
#define VULKAN_API __declspec(dllimport)
#endif // MR_VULKAN_EXPORTS

class VULKAN_API Vulkan : public IModule
{
public:
	Vulkan() = default;
	virtual ~Vulkan() noexcept = default;

	virtual bool StartupModule();




	virtual void ShutdownModule();

	virtual void Update() override;
protected:

#ifdef MR_DEBUG
	VkDebugUtilsMessengerEXT debugDispatcher;
#endif // MR_DEBUG

	bool CreateSemaphore();

	struct VkFoundDevice
	{
		VkPhysicalDevice device;
		u32 queueFamilyIndex;
	} foundDev;

	bool CreateSwapchain(VkFoundDevice& fdev);

	VkFoundDevice SelectBestCardForProject(VkPhysicalDevice* devicesList, u32 count);

	MemoryBlockPool* rendererMemoryPool;

	VkInstance instance;

	VkPhysicalDevice physicalDevice;

	VkSurfaceKHR surface;

	VkSwapchainKHR swapChain = nullptr;
	VkExtent2D swapChainExtent;

	VkDevice device;

	VkQueue graphQueue;
	VkQueue presQueue;

	u32 maxImages = 0;

	VkCommandPool cmdPool;
	VkCommandBuffer cmdBuffer;
	VkSemaphore begin, end;
	VkFence fence;

	VkImageView swapChainImageViews[8];
	VkImage swapChainImages[8];
};

//
IMPLEMENT_MODULE(Vulkan);