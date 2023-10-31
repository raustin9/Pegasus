#include "renderer.hh"
#include "core/application.hh"
#include "renderer/vkcommon.hh"
#include "renderer/debug.hh"
#include "renderer/vkdevice.hh"

#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <stdlib.h>

Renderer::Renderer(std::string name, uint32_t width, uint32_t height, Platform& platform)
    : m_title(name), 
    m_width(width), 
    m_height(height),
    m_platform(platform)
{
    m_aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    m_vkparams.Allocator = nullptr;
}

void
Renderer::OnInit() {
    InitVulkan();
}

void
Renderer::OnUpdate() {
}

void
Renderer::OnRender() {
}


// Destroy in reverse order of creation
void
Renderer::OnDestroy() {
    DestroySurface();
    DestroyInstance();
}

void 
Renderer::InitVulkan() {
    CreateInstance();
    CreateSurface();
    CreateDevice();
    GetDeviceQueue(
            m_vkparams.Device.Device, 
            m_vkparams.GraphicsQueue.FamilyIndex, 
            m_vkparams.GraphicsQueue.Handle);
    CreateSwapchain(&m_width, &m_height, Application::settings.enableVsync);
}


void 
Renderer::CreateInstance() {
    VkApplicationInfo appinfo = {};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pApplicationName = GetTitle();
    appinfo.pEngineName = GetTitle();
    appinfo.apiVersion = VK_API_VERSION_1_2;

    std::vector<const char*> instanceExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

    // TODO: platform specific ext names
#if defined(Q_PLATFORM_LINUX)
    instanceExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined (Q_PLATFORM_WINDOWS)
    instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    // Validation layer ext
    if (Application::settings.enableValidation) {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // Get the supported extensions
    std::cout << "Supported extensions:" << std::endl;
    uint32_t extensionCount = 0;
    std::vector<std::string> extensionNames;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (extensionCount > 0) {
        std::vector<VkExtensionProperties> supportedExtensions(extensionCount);

        if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, &supportedExtensions.front()) == VK_SUCCESS) {
            for (size_t i = 0; i < supportedExtensions.size(); i++) {
                std::cout << "\t" << supportedExtensions[i].extensionName << std::endl;
                extensionNames.push_back(supportedExtensions[i].extensionName);
            }
        } else {
            printf("vkEnumerateInstanceExtensionProperties did not return VK_SUCCESS\n");
            exit(1);
        }
    }

    // Create the vulkan instance
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.pApplicationInfo = &appinfo;

    // Print out required extensions
    std::cout << "Required extensions:" << std::endl;

    // Check that the extensions we need are supported
    if (instanceExtensions.size() > 0) {
        for (size_t i = 0; i < instanceExtensions.size(); i++) {
            std::cout << "\t" << instanceExtensions[i] << std::endl;
            // Output if requested ext is not available
            if (std::find(extensionNames.begin(), extensionNames.end(), instanceExtensions[i]) == extensionNames.end()) {
                printf("Extension %s not found\n", instanceExtensions[i]);
                exit(1);
            }
        }

        // set extension to enable
        createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        createInfo.ppEnabledExtensionNames = instanceExtensions.data();
    }

    // Validation layer setup
    const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
    if (Application::settings.enableValidation) {
        // Check if this layer is available at instance level
        uint32_t instanceLayerCount;
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        std::vector <VkLayerProperties> instanceLayerProps(instanceLayerCount);
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProps.data());
        bool validationPresent = false;

        for (size_t i = 0; i < instanceLayerProps.size(); i++) {
            if (strcmp(instanceLayerProps[i].layerName, validationLayerName) == 0) {
                validationPresent = true;
                break;
            }
        }

        if (validationPresent) {
            createInfo.enabledLayerCount = 1;
            createInfo.ppEnabledLayerNames = &validationLayerName;
        } else {
            std::cout << "Validation layer VK_LAYER_KHRONOS_validation not present. Validation is disabled" << std::endl;
            exit(1);
        }

        VK_CHECK(vkCreateInstance(&createInfo, m_vkparams.Allocator, &m_vkparams.Instance));

        // Set callback to handle validation
        if (Application::settings.enableValidation)
            setupDebugUtil(m_vkparams.Instance);
    }
}

// Create the window surface
void
Renderer::CreateSurface() {
    // Use platform-specific surface creation function
    m_platform.create_vulkan_surface(m_vkparams);
    std::cout << "Surface created" << std::endl;
}

// Create the swapchain
// NOTE: this is not just used on startup
//       this is also used to recreate the swapchain
//       for events that require it like 
//       window resizing
void
Renderer::CreateSwapchain(uint32_t *width, uint32_t *height, bool vsync) {
    // Store the current swap chain handle so we can use it later to ease recreation
    VkSwapchainKHR oldSwapchain = m_vkparams.SwapChain.Handle;

    // Get the physical device surface capabilities
    VkSurfaceCapabilitiesKHR surfCaps;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                m_vkparams.Device.PhysicalDevice, 
                m_vkparams.PresentationSurface, 
                &surfCaps));

    // Get the available present modes
    uint32_t presentModeCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_vkparams.Device.PhysicalDevice,
                m_vkparams.PresentationSurface,
                &presentModeCount,
                NULL));

    if (!(presentModeCount > 0))
        throw std::runtime_error("Swapchain Recreation: presentModeCount is not > 0");

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_vkparams.Device.PhysicalDevice,
                m_vkparams.PresentationSurface,
                &presentModeCount,
                presentModes.data()));

    // Select present mode for the swapchain
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    // If v-sync is not requested, try to find a mailbox mode
    // It's the lowest latency non-tearing present mode available
    if (!vsync) {
        std::cout << "Disable Vsync" << std::endl;
        for (size_t i = 0; i < presentModeCount; i++) {
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
            if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
        }
    }

    // Print the present mode
    switch(swapchainPresentMode) {
        case VK_PRESENT_MODE_MAILBOX_KHR:
              std::cout << "Present Mode: Mailbox" << std::endl;
              break;
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
              std::cout << "Present Mode: Immediate" << std::endl;
              break;
        case VK_PRESENT_MODE_FIFO_KHR:
              std::cout << "Present Mode: FIFO" << std::endl;
              break;
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
              std::cout << "Present Mode: FIFO Relaxed" << std::endl;
              break;
        case VK_PRESENT_MODE_MAX_ENUM_KHR:
              std::cout << "Present Mode: MAX ENUM" << std::endl;
              break;
        case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
              std::cout << "Present Mode: Shared Demand Refresh" << std::endl;
              break;
        case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
              std::cout << "Present Mode: Continuous Refresh" << std::endl;
              break;
        default:
              std::cout << "Present Mode: Unknown" << std::endl;
              break;
    }

    // Determine the number of images and set the number of command buffers
    uint32_t desiredSwapchainImageCount = m_command_buffer_count = surfCaps.minImageCount+1;
    if ( (surfCaps.maxImageCount > 0) && (desiredSwapchainImageCount > surfCaps.maxImageCount) ) {
        desiredSwapchainImageCount = surfCaps.maxImageCount;
    }

    // Find a surface-supported transformation to apply the image prior to presentation
    VkSurfaceTransformFlagsKHR preTransform;
    if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        // We prefer a non-rotated transform
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        // otherwise, use the current transform relative to the presentation engine's natural orientaion
        preTransform = surfCaps.currentTransform;
    }

    // Get the extend
    VkExtent2D swapchainExtent = {};

    // if the width and height equal the special value of 0xFFFFFFFF,
    // the size of the surface is undefined
    if (surfCaps.currentExtent.width == (uint32_t)-1) {
        // The size is set to the size of the window's client area
        swapchainExtent.width = *width;
        swapchainExtent.height = *height;
    } else {
        // If the surface size is defined, the size of the swapchain images must match
        swapchainExtent = surfCaps.currentExtent;

        // Save the result in case the inferred surface size and the size of the client area mismatch
        *width = surfCaps.currentExtent.width;
        *height = surfCaps.currentExtent.height;
    }

    // Save the size of the swapchain images
    m_vkparams.SwapChain.Extent = swapchainExtent;

    // Get list of supported surface formats
    uint32_t formatCount;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_vkparams.Device.PhysicalDevice,
                m_vkparams.PresentationSurface, 
                &formatCount,
                NULL));

    if (!(formatCount > 0))
        throw std::runtime_error("Swapchain Recreation: formatCount is not > 0");

    std::vector <VkSurfaceFormatKHR> surfaceFormats(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_vkparams.Device.PhysicalDevice,
                m_vkparams.PresentationSurface, 
                &formatCount,
                surfaceFormats.data()));

    // Iterate over the list of available surface format and check for the presence of a 
    // four-component, 32-bit unsigned normalized format with 8 bits per component
    bool preferredFormatFound = false;
    for (size_t i = 0; i < surfaceFormats.size(); i++) {
        if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM
            || surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM) {
            m_vkparams.SwapChain.Format = surfaceFormats[i].format;
            m_vkparams.SwapChain.ColorSpace = surfaceFormats[i].colorSpace;
            preferredFormatFound = true;
            break;
        }
    }

    // Could not find our preferred formats
    // Falling back to the first format exposed
    // Rendering may be incorrect
    if (!preferredFormatFound) {
        m_vkparams.SwapChain.Format = surfaceFormats[0].format;
        m_vkparams.SwapChain.ColorSpace = surfaceFormats[0].colorSpace;
        std::cout << "WARNING: unable to find preferred surface format. Rendering may be incorrect" << std::endl;
    }

    // Find a supported composite alpha-mode (not all devices support alpha opaque)
    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    // Select the first alpha composite mode available
    std::vector <VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (size_t i = 0; i < compositeAlphaFlags.size(); i++) {
        if (surfCaps.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_vkparams.PresentationSurface;
    createInfo.minImageCount = desiredSwapchainImageCount;
    createInfo.imageFormat = m_vkparams.SwapChain.Format;
    createInfo.imageColorSpace = m_vkparams.SwapChain.ColorSpace;
    createInfo.imageExtent = { swapchainExtent.width, swapchainExtent.height };
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
    createInfo.imageArrayLayers = 1;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.presentMode = swapchainPresentMode;
    // Setting oldwapchain to the saved handle of the previous swapchain aids resource reuse
    // and makes sure that we can still present already acquired images
    createInfo.oldSwapchain = oldSwapchain;
    // Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
    createInfo.clipped = VK_TRUE;
    createInfo.compositeAlpha = compositeAlpha;

    // Enable transfer source on swap chain images if supported
    if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        createInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    VK_CHECK(vkCreateSwapchainKHR(
                m_vkparams.Device.Device,
                &createInfo,
                m_vkparams.Allocator,
                &m_vkparams.SwapChain.Handle));

    // If an existing swapchain is re-created, destroy the old swapchain
    if (oldSwapchain != VK_NULL_HANDLE) {
        for (uint32_t i = 0; i < m_vkparams.SwapChain.Images.size(); i++) {
            vkDestroyImageView(m_vkparams.Device.Device, m_vkparams.SwapChain.Images[i].View, m_vkparams.Allocator);
        }

        vkDestroySwapchainKHR(m_vkparams.Device.Device, oldSwapchain, m_vkparams.Allocator);
    }

    uint32_t imageCount = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(m_vkparams.Device.Device, m_vkparams.SwapChain.Handle, &imageCount, NULL));

    m_vkparams.SwapChain.Images.resize(imageCount);
    std::vector<VkImage> images(imageCount);
    VK_CHECK(vkGetSwapchainImagesKHR(m_vkparams.Device.Device, m_vkparams.SwapChain.Handle, &imageCount, images.data()));

    VkImageViewCreateInfo colorAttachmentView = {};
    colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    colorAttachmentView.format = m_vkparams.SwapChain.Format;
    colorAttachmentView.components = {
        VK_COMPONENT_SWIZZLE_R,
        VK_COMPONENT_SWIZZLE_G,
        VK_COMPONENT_SWIZZLE_B,
        VK_COMPONENT_SWIZZLE_A,
    };

    colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorAttachmentView.subresourceRange.baseMipLevel = 0;
    colorAttachmentView.subresourceRange.levelCount = 1;
    colorAttachmentView.subresourceRange.baseArrayLayer = 0;
    colorAttachmentView.subresourceRange.layerCount = 1;
    colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;

    // Create the image views, and save them (along with the image objects)
    for (uint32_t i = 0; i < imageCount; i++) {
        m_vkparams.SwapChain.Images[i].Handle = images[i];
        colorAttachmentView.image = m_vkparams.SwapChain.Images[i].Handle;
        VK_CHECK(vkCreateImageView(m_vkparams.Device.Device, &colorAttachmentView, m_vkparams.Allocator, &m_vkparams.SwapChain.Images[i].View));
    }
}

void
Renderer::CreateDevice() {
    // Get the physical device
    uint32_t gpuCount = 0;

    // Get number of physical devices
    vkEnumeratePhysicalDevices(m_vkparams.Instance, &gpuCount, nullptr);
    if (gpuCount == 0) {
        std::cout << "No device with Vulkan support found" << std::endl;
        exit(1);
    }

    // Enumerate the devices
    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
    VkResult err = vkEnumeratePhysicalDevices(m_vkparams.Instance, &gpuCount, &physicalDevices.front());
    if (err) {
        throw std::runtime_error("Could not enumerate physical devices\n");
    }

    // Select physical device that has a graphics queue
    for (size_t i = 0; i < gpuCount; i++) {
        if (CheckPhysicalDeviceProperties(physicalDevices[i], m_vkparams)) {
            m_vkparams.Device.PhysicalDevice = physicalDevices[i];
            vkGetPhysicalDeviceProperties(m_vkparams.Device.PhysicalDevice, &m_deviceProperties);
            break;
        }
    }

    // Make sure we have a valid physical device
    if (m_vkparams.Device.PhysicalDevice == VK_NULL_HANDLE
        || m_vkparams.GraphicsQueue.FamilyIndex == UINT32_MAX) {
        throw std::runtime_error("Could not select physical device based on chosen properties\n");
    } else {
        vkGetPhysicalDeviceFeatures(m_vkparams.Device.PhysicalDevice, &m_vkparams.Device.DeviceFeatures);
        vkGetPhysicalDeviceMemoryProperties(m_vkparams.Device.PhysicalDevice, &m_vkparams.Device.DeviceMemoryProperties);
    }

    // Desired queues need to be requested upon logical devices
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

    // Array of normalized vector floating point values (between 0 and 1 inclusive)
    // specifying priotities of work to each requested queue.
    // Higher vals mean higher prio with 0.0 being the lowest
    // Within the same device, queues with higher prio may be allotted more 
    // processing time than queues with lower prio
    const float queuePriorities[] = {1.0f};

    // Request a single graphics queue
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = m_vkparams.GraphicsQueue.FamilyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriorities;
    queueCreateInfos.push_back(queueInfo);

    // Add swapchain extension
    std::vector <const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Get the list of supported device extensions
    uint32_t extCount = 0;
    std::vector <std::string> supportedDeviceExtensions;
    vkEnumerateDeviceExtensionProperties(m_vkparams.Device.PhysicalDevice, nullptr, &extCount, nullptr);
    if (extCount > 0) {
        std::vector <VkExtensionProperties> extensions(extCount);
        if (vkEnumerateDeviceExtensionProperties(m_vkparams.Device.PhysicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS) {
            for (size_t i = 0; i < extensions.size(); i++) {
                supportedDeviceExtensions.push_back(extensions[i].extensionName);
            }
        }
    }


    // Create the logical device
    if (CreateLogicalDevice(queueCreateInfos, deviceExtensions, supportedDeviceExtensions, m_vkparams) != VK_SUCCESS) {
        throw std::runtime_error("CreateLogicalDevice() could not create vulkan logical device");
    }

    std::cout << "Device created" << std::endl;
}


//
// Destroy Vulkan items
//


void
Renderer::DestroyInstance() {
    // TODO: destroy vulkan instance
}


void
Renderer::DestroySurface() {
    // TODO: destroy vulkan surface 
}
