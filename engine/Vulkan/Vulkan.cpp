/* Copyright 2020 - 2025, Hansson Software. All rights reserved. */

#include "Vulkan.h"
#include <Special/EngineConstants.h>
#include <Application/Application.h>
#include <Memory/MemoryHandler.h>
#include <Memory/MemoryBlockPool.h>
#include <HAL/Window.h>
#include <HAL/File.h>

#ifdef MR_PLATFORM_WINDOWS
#include <HAL/Win32/MinimalWin.h>
#include <vulkan/vulkan_win32.h>
#endif // MR_PLATFORM_WINDOWS

LOG_ADDCATEGORY(Renderer);

static constexpr const char* VkResultToStr(const VkResult& result);

static Array<VkImage> images;
static Array<VkImageView> imageViews;

static Array<const char*> enabledLayers;

static Array<const char*> enabledInstanceExtensions;

static Array<const char*> enabledExtensions;

bool Vulkan::StartupModule()
{
    moduleState = ModuleState::Loading;
    rendererMemoryPool = GetMemoryManager()->RequestNewRegion<MemoryBlockPool>("VulkanRendererRegion", 256_mB);

    if (!rendererMemoryPool)
    {
        MR_LOG(LogRenderer, Fatal, "Failed to reserve memory for Renderer!");
        return false;
    }

#ifdef MR_DEBUG
    enabledLayers.Add("VK_LAYER_KHRONOS_validation");
    enabledInstanceExtensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // MR_DEBUG
 
    enabledInstanceExtensions.Add(VK_KHR_SURFACE_EXTENSION_NAME);
    enabledExtensions.Add(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#ifdef MR_PLATFORM_WINDOWS
    enabledInstanceExtensions.Add(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif // MR_PLATFORM_WINDOWS

    u32 allExtensionsCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &allExtensionsCount, nullptr);

    VkExtensionProperties extensions_s[16] = {};
    VkExtensionProperties* extensions = allExtensionsCount > 16 ? (VkExtensionProperties*)GetMemoryManager()->Allocate(allExtensionsCount * sizeof(VkExtensionProperties), rendererMemoryPool) : extensions_s;
    vkEnumerateInstanceExtensionProperties(nullptr, &allExtensionsCount, extensions);

    u32 compatible = 0;
    for (const char*& extensionName : enabledInstanceExtensions)
    {
        for (u32 i = 0; i < allExtensionsCount; i++)
        {
            VkExtensionProperties* equippedExtension = &extensions[i];
            if (strcmp(extensionName, equippedExtension->extensionName) == 0)
            {
                compatible++;
                break;
            }
        }
    }

    if (compatible != enabledInstanceExtensions.GetSize())
    {
        MR_LOG(LogRenderer, Fatal, "One or more extensions are not compatible with your gpu!");
        return false;
    }

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
        enabledInstanceExtensions.GetSize(),        // uint32_t                    enabledExtensionCount
        enabledInstanceExtensions.Data()            // const char* const*          ppEnabledExtensionNames
    };

    const VkResult result1 = vkCreateInstance(&instanceInfo, nullptr, &instance);
    if (result1 != VK_SUCCESS)
    {
        MR_LOG(LogRenderer, Error, "vkCreateInstance returned: %s", VkResultToStr(result1));
        return false;
    }

    // TODO: Debug messenger code goes here!

#ifdef MR_PLATFORM_WINDOWS
    VkWin32SurfaceCreateInfoKHR surfaceInfo = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    surfaceInfo.hinstance = (HINSTANCE)GetApplication()->GetApplicationNativeHandle();
    surfaceInfo.hwnd = (HWND)GetApplication()->GetMainWindow()->GetNativeHandle();

    const VkResult result2 = vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, &surface);
    if (result2 != VK_SUCCESS)
    {
        MR_LOG(LogRenderer, Error, "vkCreateInstance returned: %s", VkResultToStr(result2));
        return false;
    }
#else

#endif // MR_PLATFORM_WINDOWS


    u32 physicalDevicesCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr);

    VkPhysicalDevice physicalDevices_s[4] = {};
    VkPhysicalDevice* physicalDevices = physicalDevicesCount > 4 ? (VkPhysicalDevice*)GetMemoryManager()->Allocate(physicalDevicesCount * sizeof(VkPhysicalDevice), rendererMemoryPool) : physicalDevices_s;
    vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, physicalDevices);

    VkFoundDevice fdev = SelectBestCardForProject(physicalDevices, physicalDevicesCount);

    constexpr const float priority[] = {0.f, 1.f};
    VkDeviceQueueCreateInfo queueInfo = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueInfo.pQueuePriorities = priority;
    queueInfo.queueFamilyIndex = fdev.queueFamilyIndex;
    queueInfo.queueCount = 2;

    VkDeviceCreateInfo devInfo = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    devInfo.pQueueCreateInfos = &queueInfo;
    devInfo.queueCreateInfoCount = 1;
    devInfo.enabledExtensionCount = enabledExtensions.GetSize();
    devInfo.ppEnabledExtensionNames = enabledExtensions.Data();

    const VkResult result3 = vkCreateDevice(fdev.device, &devInfo, nullptr, &device);
    if (result3 != VK_SUCCESS)
    {
        MR_LOG(LogRenderer, Error, "vkCreateDevice returned: %s", VkResultToStr(result3));
        return false;
    }

    vkGetDeviceQueue(device, fdev.queueFamilyIndex, 0, &graphQueue);

    VkSurfaceCapabilitiesKHR surfaceCaps = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(fdev.device, surface, &surfaceCaps);

    maxImages = surfaceCaps.minImageCount + 1;

    VkSwapchainCreateInfoKHR swapChainInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapChainInfo.surface = surface;
    swapChainInfo.minImageCount = maxImages;
    swapChainInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    swapChainInfo.imageExtent = surfaceCaps.currentExtent;
    swapChainInfo.imageArrayLayers = 1;
    swapChainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainInfo.preTransform = surfaceCaps.currentTransform;
    swapChainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainInfo.clipped = VK_TRUE;


    const VkResult result4 = vkCreateSwapchainKHR(device, &swapChainInfo, nullptr, &swapChain);
    if (result4 != VK_SUCCESS)
    {
        MR_LOG(LogRenderer, Error, "vkCreateSwapchainKHR returned: %s", VkResultToStr(result4));
        return false;
    }

    VkImage swapChainImages_s[8] = {};
    VkImage* swapChainImages = maxImages > 8 ? (VkImage*)GetMemoryManager()->Allocate(maxImages * sizeof(VkImage), rendererMemoryPool) : swapChainImages_s;
    vkGetSwapchainImagesKHR(device, swapChain, &maxImages, swapChainImages);

    VkImageView swapChainImageViews_s[8] = {};
    VkImageView* swapChainImageViews = maxImages > 8 ? (VkImageView*)GetMemoryManager()->Allocate(maxImages * sizeof(VkImageView), rendererMemoryPool) : swapChainImageViews_s;

    for (u32 i = 0; i < maxImages; i++)
    {
        constexpr VkImageSubresourceRange range = 
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            1
        };


        VkImageViewCreateInfo imgInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        imgInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imgInfo.image = swapChainImages[i];
        imgInfo.subresourceRange = range;
        imgInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        const VkResult resultX = vkCreateImageView(device, &imgInfo, nullptr, &swapChainImageViews[i]);
        if (resultX != VK_SUCCESS)
        {
            MR_LOG(LogRenderer, Error, "vkCreateImageView returned: %s", VkResultToStr(resultX));
            return false;
        }
    }

    File vertexShader;
    vertexShader.Open("vertex.spv", OpenAs::JustOpen, Access::Read, Share::Reading);
    char big[2048] = {};
    vertexShader.Read(big);

    VkShaderModule modules[2];
    VkShaderModuleCreateInfo shaderModuleInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    shaderModuleInfo.pCode = (const uint32_t*)(big);
    shaderModuleInfo.codeSize = vertexShader.GetFileSize();

    vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &modules[0]);

    VkPipelineShaderStageCreateInfo shaderStage = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    shaderStage.pName = "main";
    shaderStage.module = modules[0];
    shaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;

    File fragmentShader;
    fragmentShader.Open("fragment.spv", OpenAs::JustOpen, Access::Read, Share::Reading);
    char bigA[2048] = {};
    fragmentShader.Read(bigA);

    VkShaderModuleCreateInfo shaderModuleInfoA = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    shaderModuleInfoA.pCode = (const uint32_t*)(bigA);
    shaderModuleInfoA.codeSize = fragmentShader.GetFileSize();

    vkCreateShaderModule(device, &shaderModuleInfoA, nullptr, &modules[1]);

    VkPipelineShaderStageCreateInfo shaderStageA = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    shaderStageA.pName = "main";
    shaderStageA.module = modules[1];
    shaderStageA.stage = VK_SHADER_STAGE_FRAGMENT_BIT;


    VkPipelineLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    
    VkPipelineLayout pipelineLayout;
    vkCreatePipelineLayout(device, &layoutInfo, nullptr, &pipelineLayout);

    VkRenderPass renderPass = VK_NULL_HANDLE;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        MR_LOG(LogRenderer, Error, "Failed to create render pass!");
    }

    // --- Vertex input state ---
    VkVertexInputBindingDescription bindingDesc = {};
    bindingDesc.binding = 0;
    bindingDesc.stride = sizeof(float) * 5; // 2 pos + 3 color
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attrDescs[2] = {};
    attrDescs[0].location = 0;
    attrDescs[0].binding = 0;
    attrDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
    attrDescs[0].offset = 0;
    attrDescs[1].location = 1;
    attrDescs[1].binding = 0;
    attrDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attrDescs[1].offset = sizeof(float) * 2;

    VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {};
    vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateInfo.pVertexBindingDescriptions = &bindingDesc;
    vertexInputStateInfo.vertexAttributeDescriptionCount = 2;
    vertexInputStateInfo.pVertexAttributeDescriptions = attrDescs;

    // --- Input assembly ---
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // --- Viewport & scissor (dynamic) ---
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    // --- Rasterization ---
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.lineWidth = 1.0f;

    // --- Multisample ---
    VkPipelineMultisampleStateCreateInfo multisample = {};
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // --- Color blend ---
    VkPipelineColorBlendAttachmentState colorBlendAtt = {};
    colorBlendAtt.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAtt.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlend = {};
    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.attachmentCount = 1;
    colorBlend.pAttachments = &colorBlendAtt;

    VkPipelineShaderStageCreateInfo stageInofs[] = { shaderStage, shaderStageA };

    // --- Main pipeline info ---
    VkGraphicsPipelineCreateInfo pplInfo = {};
    pplInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pplInfo.stageCount = 2;
    pplInfo.pStages = stageInofs;
    pplInfo.pVertexInputState = &vertexInputStateInfo;
    pplInfo.pInputAssemblyState = &inputAssembly;
    pplInfo.pViewportState = &viewportState;
    pplInfo.pRasterizationState = &rasterizer;
    pplInfo.pMultisampleState = &multisample;
    pplInfo.pColorBlendState = &colorBlend;
    pplInfo.pDynamicState = &dynamicState;
    pplInfo.layout = pipelineLayout;     // ezt korábban létre kell hozni
    pplInfo.renderPass = renderPass;     // ezt korábban létre kell hozni
    pplInfo.subpass = 0;
    pplInfo.basePipelineHandle = VK_NULL_HANDLE;
    pplInfo.basePipelineIndex = -1;   

    VkPipeline pp;
    const VkResult result5 = vkCreateGraphicsPipelines(device, nullptr, 1, &pplInfo, nullptr, &pp);
    if (result5 != VK_SUCCESS)
    {
        MR_LOG(LogRenderer, Error, "vkCreateGraphicsPipelines returned: %s", VkResultToStr(result5));
        return false;
    }

    moduleState = ModuleState::Enabled;
    return true;
}

void Vulkan::ShutdownModule()
{
    vkDestroySwapchainKHR(device, swapChain, nullptr);

    vkDestroyDevice(device, nullptr);

    vkDestroySurfaceKHR(instance, surface, nullptr);

	vkDestroyInstance(instance, nullptr);
    
    moduleState = ModuleState::Unloaded;
}

void Vulkan::Update()
{
  

}

Vulkan::VkFoundDevice Vulkan::SelectBestCardForProject(VkPhysicalDevice* devicesList, u32 count)
{
    bool bFound = false;
    VkFoundDevice best = {};

    for (u32 i = 0; i < count; i++)
    {
        VkPhysicalDevice& indexed = devicesList[i];

        u32 queueFamilyPropsCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(indexed, &queueFamilyPropsCount, nullptr);

        VkQueueFamilyProperties queueFamilyProps_s[8] = {};
        VkQueueFamilyProperties* queueFamilyProps = queueFamilyPropsCount > 8 ? (VkQueueFamilyProperties*)GetMemoryManager()->Allocate(queueFamilyPropsCount * sizeof(VkQueueFamilyProperties), rendererMemoryPool) : queueFamilyProps_s;

        vkGetPhysicalDeviceQueueFamilyProperties(indexed, &queueFamilyPropsCount, queueFamilyProps);
            
        for (u32 j = 0; j < count; j++)
        {
            VkQueueFamilyProperties& indexedFamilyProp = queueFamilyProps[i];
            if (indexedFamilyProp.queueFlags & VK_QUEUE_GRAPHICS_BIT && vkGetPhysicalDeviceWin32PresentationSupportKHR(indexed, j))
            {
                best.device = indexed;
                best.queueFamilyIndex = j;

                bFound = true;
                break;
            }
        }

        if (bFound)
        {
            break;
        }
    }

    return best;
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