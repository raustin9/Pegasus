#include "vkswapchain.hh"

void 
RecreateSwapchain(
        VKCommonParameters &params, 
        uint32_t* width, 
        uint32_t *height, 
        bool vsync, 
        uint32_t &commandBufferCount
) {
    // Store the current swap chain handle so we can use it later to ease recreation
    VkSwapchainKHR oldSwapchain = params.SwapChain.Handle;

    // Get the physical device surface capabilities
    VkSurfaceCapabilitiesKHR surfCaps;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                params.Device.PhysicalDevice, 
                params.PresentationSurface, 
                &surfCaps));

    // Get the available present modes
    uint32_t presentModeCount = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
                params.Device.PhysicalDevice,
                params.PresentationSurface,
                &presentModeCount,
                NULL));

    if (!(presentModeCount > 0))
        throw std::runtime_error("Swapchain Recreation: presentModeCount is not > 0");

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
                params.Device.PhysicalDevice,
                params.PresentationSurface,
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
    uint32_t desiredSwapchainImageCount = commandBufferCount  = surfCaps.minImageCount+1;
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

    // Get the extent
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
    params.SwapChain.Extent = swapchainExtent;

    // Get list of supported surface formats
    uint32_t formatCount;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
                params.Device.PhysicalDevice,
                params.PresentationSurface, 
                &formatCount,
                NULL));

    if (!(formatCount > 0))
        throw std::runtime_error("Swapchain Recreation: formatCount is not > 0");

    std::vector <VkSurfaceFormatKHR> surfaceFormats(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
                params.Device.PhysicalDevice,
                params.PresentationSurface, 
                &formatCount,
                surfaceFormats.data()));

    // Iterate over the list of available surface format and check for the presence of a 
    // four-component, 32-bit unsigned normalized format with 8 bits per component
    bool preferredFormatFound = false;
    for (size_t i = 0; i < surfaceFormats.size(); i++) {
        if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM
            || surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM) {
            params.SwapChain.Format = surfaceFormats[i].format;
            params.SwapChain.ColorSpace = surfaceFormats[i].colorSpace;
            preferredFormatFound = true;
            break;
        }
    }

    // Could not find our preferred formats
    // Falling back to the first format exposed
    // Rendering may be incorrect
    if (!preferredFormatFound) {
        params.SwapChain.Format = surfaceFormats[0].format;
        params.SwapChain.ColorSpace = surfaceFormats[0].colorSpace;
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
    createInfo.surface = params.PresentationSurface;
    createInfo.minImageCount = desiredSwapchainImageCount;
    createInfo.imageFormat = params.SwapChain.Format;
    createInfo.imageColorSpace = params.SwapChain.ColorSpace;
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
                params.Device.Device,
                &createInfo,
                params.Allocator,
                &params.SwapChain.Handle));

    // If an existing swapchain is re-created, destroy the old swapchain
    if (oldSwapchain != VK_NULL_HANDLE) {
        for (uint32_t i = 0; i < params.SwapChain.Images.size(); i++) {
            vkDestroyImageView(params.Device.Device, params.SwapChain.Images[i].View, params.Allocator);
        }

        vkDestroySwapchainKHR(params.Device.Device, oldSwapchain, params.Allocator);
    }

    uint32_t imageCount = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(params.Device.Device, params.SwapChain.Handle, &imageCount, NULL));

    params.SwapChain.Images.resize(imageCount);
    std::vector<VkImage> images(imageCount);
    VK_CHECK(vkGetSwapchainImagesKHR(params.Device.Device, params.SwapChain.Handle, &imageCount, images.data()));

    VkImageViewCreateInfo colorAttachmentView = {};
    colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    colorAttachmentView.format = params.SwapChain.Format;
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
        params.SwapChain.Images[i].Handle = images[i];
        colorAttachmentView.image = params.SwapChain.Images[i].Handle;
        VK_CHECK(vkCreateImageView(params.Device.Device, &colorAttachmentView, params.Allocator, &params.SwapChain.Images[i].View));
    }

}
