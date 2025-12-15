/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Vulkan.h"
#include <Platform/PlatformLayout.h>

#ifdef MR_PLATFORM_WINDOWS
#include <vulkan/vulkan_win32.h>
#endif // MR_PLATFORM_WINDOWS


void Vulkan::StartupModule()
{
	Renderer::StartupModule();
#ifdef MR_DEBUG
    enabledLayers.Add("VK_LAYER_KHRONOS_validation");
    enabledExtensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // MR_DEBUG
 
    enabledExtensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#ifdef MR_PLATFORM_WINDOWS
    enabledExtensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif // MR_PLATFORM_WINDOWS


    constexpr VkApplicationInfo appInfo = 
    {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,     // VkStructureType    sType;
        nullptr,                                // const void*        pNext;
        nullptr,                                // const char*        pApplicationName;
        0,                                      // uint32_t           applicationVersion;
        nullptr,                                // const char*        pEngineName;
        0,                                      // uint32_t           engineVersion;
        VK_API_VERSION_1_2                      // uint32_t           apiVersion;
    };

    const VkInstanceCreateInfo instanceInfo =
    {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,     // VkStructureType             sType
        nullptr,                                    // const void*                 pNext
        0,                                          // VkInstanceCreateFlags       flags
        &appInfo,                                   // const VkApplicationInfo*    pApplicationInfo
#ifdef MR_DEBUG
        enabledLayers.GetSize(),                    // uint32_t                    enabledLayerCount
        enabledLayers.Data(),                       // const char* const*          ppEnabledLayerNames
#else
        0,                                          // uint32_t                    enabledLayerCount
        nullptr,                                    // const char* const*          ppEnabledLayerNames
#endif // MR_DEBUG                                         
        enabledExtensions.GetSize(),                // uint32_t                    enabledExtensionCount
        enabledExtensions.Data()                    // const char* const*          ppEnabledExtensionNames
    };

    vkCreateInstance(&instanceInfo, nullptr, &instance);
}

void Vulkan::ShutdownModule()
{
	Renderer::ShutdownModule();

	vkDestroyInstance(instance, nullptr);
}

void Vulkan::Update()
{
    Renderer::Update();
}
