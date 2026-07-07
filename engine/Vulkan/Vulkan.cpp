/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Vulkan.h"
#include <Special/EngineConstants.h>
#include <Application/Application.h>

#ifdef MR_PLATFORM_WINDOWS
#include <HAL/Win32/MinimalWin.h>
#include <vulkan/vulkan_win32.h>
#endif // MR_PLATFORM_WINDOWS

LOG_ADDCATEGORY(Renderer);

static constexpr const char* VkResultToStr(const VkResult& result);

void Vulkan::StartupModule()
{
    if (!GetMemoryManager()->RequestNewRegion("VulkanRendererRegion", 256_mB))
    {
        MR_LOG(LogRenderer, Fatal, "Failed to reserve memory for Renderer!");
        return;
    }

#ifdef MR_DEBUG
    enabledLayers.Add("VK_LAYER_KHRONOS_validation");
    //enabledExtensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // MR_DEBUG
 
    enabledExtensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#ifdef MR_PLATFORM_WINDOWS
    enabledExtensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif // MR_PLATFORM_WINDOWS

    u32 allExtensionsCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &allExtensionsCount, nullptr);

    VkExtensionProperties extensions_s[16] = {};
    VkExtensionProperties* extensions = allExtensionsCount > 16 ? (VkExtensionProperties*)GetMemoryManager()->Allocate(allExtensionsCount * sizeof(VkExtensionProperties)) : extensions_s;
    vkEnumerateInstanceExtensionProperties(nullptr, &allExtensionsCount, nullptr);


    const VkApplicationInfo appInfo = 
    {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,     // VkStructureType    sType;
        nullptr,                                // const void*        pNext;
        GetApplication()->GetApplicationName()->Chr(), // const char*        pApplicationName;
        0,                                      // uint32_t           applicationVersion;
        ENGINE_NAME_SPACE,                      // const char*        pEngineName;
        0,                                      // uint32_t           engineVersion;
        VK_API_VERSION_1_4                      // uint32_t           apiVersion;
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

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
    if (result != VK_SUCCESS)
    {
        MR_LOG(LogRenderer, Error, "vkCreateInstance returned: %s", VkResultToStr(result));
    }
}

void Vulkan::ShutdownModule()
{

	vkDestroyInstance(instance, nullptr);
}

void Vulkan::Update()
{
  
}

static constexpr const char* VkResultToStr(const VkResult& result)
{
    switch (result)
    {
     case VK_SUCCESS:
        return "VK_SUCCESS";
     case VK_NOT_READY:
        return "VK_NOT_READY";
     case VK_TIMEOUT:
        return "VK_TIMEOUT";
     case VK_EVENT_SET:
        return "VK_EVENT_SET";
     case VK_EVENT_RESET:
        return "VK_EVENT_RESET";
     case VK_INCOMPLETE:
        return "VK_INCOMPLETE";
     case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "VK_ERROR_OUT_OF_HOST_MEMORY";
     case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
     case VK_ERROR_INITIALIZATION_FAILED:
        return "VK_ERROR_INITIALIZATION_FAILED";
     case VK_ERROR_DEVICE_LOST:
        return "VK_ERROR_DEVICE_LOST";
     case VK_ERROR_MEMORY_MAP_FAILED:
        return "VK_ERROR_MEMORY_MAP_FAILED";
     case VK_ERROR_LAYER_NOT_PRESENT:
        return "VK_ERROR_LAYER_NOT_PRESENT";
     case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "VK_ERROR_EXTENSION_NOT_PRESENT";
     case VK_ERROR_FEATURE_NOT_PRESENT:
        return "VK_ERROR_FEATURE_NOT_PRESENT";
     case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "VK_ERROR_INCOMPATIBLE_DRIVER";
     case VK_ERROR_TOO_MANY_OBJECTS:
        return "VK_ERROR_TOO_MANY_OBJECTS";
     case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "VK_ERROR_FORMAT_NOT_SUPPORTED";
     case VK_ERROR_FRAGMENTED_POOL:
        return "VK_ERROR_FRAGMENTED_POOL";
     case VK_ERROR_UNKNOWN:
        return "VK_ERROR_UNKNOWN";
     case VK_ERROR_VALIDATION_FAILED:
        return "VK_ERROR_VALIDATION_FAILED";
     case VK_ERROR_OUT_OF_POOL_MEMORY:
        return "VK_ERROR_OUT_OF_POOL_MEMORY";
     case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
     case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
     case VK_ERROR_FRAGMENTATION:
        return "VK_ERROR_FRAGMENTATION";
     case VK_PIPELINE_COMPILE_REQUIRED:
        return "VK_PIPELINE_COMPILE_REQUIRED";
     case VK_ERROR_NOT_PERMITTED:
        return "VK_ERROR_NOT_PERMITTED";
     case VK_ERROR_SURFACE_LOST_KHR:
        return "VK_ERROR_SURFACE_LOST_KHR";
     case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
     case VK_SUBOPTIMAL_KHR:
        return "VK_SUBOPTIMAL_KHR";
     case VK_ERROR_OUT_OF_DATE_KHR:
        return "VK_ERROR_OUT_OF_DATE_KHR";
     case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
     case VK_ERROR_INVALID_SHADER_NV:
        return "VK_ERROR_INVALID_SHADER_NV";
     case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
        return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
     case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
     case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
     case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
     case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
     case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
        return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
     case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
     case VK_ERROR_PRESENT_TIMING_QUEUE_FULL_EXT:
        return "VK_ERROR_PRESENT_TIMING_QUEUE_FULL_EXT";
     case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
     case VK_THREAD_IDLE_KHR:
        return "VK_THREAD_IDLE_KHR";
     case VK_THREAD_DONE_KHR:
        return "VK_THREAD_DONE_KHR";
     case VK_OPERATION_DEFERRED_KHR:
        return "VK_OPERATION_DEFERRED_KHR";
     case VK_OPERATION_NOT_DEFERRED_KHR:
        return "VK_OPERATION_NOT_DEFERRED_KHR";
     case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
        return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
     case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
        return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
     case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
        return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
     case VK_PIPELINE_BINARY_MISSING_KHR:
        return "VK_PIPELINE_BINARY_MISSING_KHR";
     case VK_ERROR_NOT_ENOUGH_SPACE_KHR:
        return "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
    default:
        return "Unknown";
    }

    return "";
}