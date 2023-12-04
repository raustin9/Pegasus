#include "vulkan_backend.hh"
#include "vk_device.hh"
#include "vk_image.hh"

void create(VKContext& context, uint32_t width, uint32_t height, VKSwapchain& swapchain);
void destroy(VKContext& context, VKSwapchain& swapchain);

bool 
VulkanBackend::create_swapchain(uint32_t width, uint32_t height, VKSwapchain& out_swapchain) {
    // Create a new one
    create(m_context, width, height, out_swapchain);
    return true;
}

bool
VulkanBackend::recreate_swapchain(uint32_t width, uint32_t height, VKSwapchain& swapchain) {
    // Destroy old swapchain and crete a new one
    destroy(m_context, swapchain);
    create(m_context, width, height, swapchain);
    return true;
}

void
VulkanBackend::destroy_swapchain() {
    destroy(m_context, m_context.swapchain);
}

void
create(VKContext& context, uint32_t width, uint32_t height, VKSwapchain& swapchain) {
    VkExtent2D swapchain_extent = {width, height};
    swapchain.max_frames_in_flight = 2;

    bool found = false;
    for (uint32_t i = 0; i < context.device.swapchain_support.format_count; i++) {
        VkSurfaceFormatKHR format = context.device.swapchain_support.formats[i];
        // Preferred formats
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM 
            && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            swapchain.image_format = format;
            found = true;
            break;
        }
    }

    if (!found) {
        swapchain.image_format = context.device.swapchain_support.formats[0];
    }

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32_t i = 0; i < context.device.swapchain_support.present_mode_count; i++) {
        VkPresentModeKHR mode = context.device.swapchain_support.present_modes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            // Most desired mode
            present_mode = mode;
            break;
        }
    }

    vkdevice_query_swapchain_support(
        context.device,
        context.surface,
        context.device.swapchain_support
    );

    // If a device does not support the current extent, overwrite it with something that is
    if (context.device.swapchain_support.capabilities.currentExtent.width != UINT32_MAX) {
        swapchain_extent = context.device.swapchain_support.capabilities.currentExtent;
    }

    VkExtent2D min = context.device.swapchain_support.capabilities.minImageExtent;
    VkExtent2D max = context.device.swapchain_support.capabilities.maxImageExtent;
    swapchain_extent.width = std::clamp(swapchain_extent.width, min.width, max.width);
    swapchain_extent.height = std::clamp(swapchain_extent.height, min.height, max.height);

    uint32_t image_count = context.device.swapchain_support.capabilities.minImageCount+1;
    if (context.device.swapchain_support.capabilities.maxImageCount > 0
        && image_count > context.device.swapchain_support.capabilities.maxImageCount) {
        image_count = context.device.swapchain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_info = {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = context.surface;
    swapchain_info.minImageCount = image_count;
    swapchain_info.imageFormat = swapchain.image_format.format;
    swapchain_info.imageColorSpace = swapchain.image_format.colorSpace;
    swapchain_info.imageExtent = swapchain_extent;
    swapchain_info.imageArrayLayers = 1;
    swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (context.device.graphics_queue_index != context.device.present_queue_index) {
        uint32_t queue_family_indices[] = {
            static_cast<uint32_t>(context.device.graphics_queue_index),
            static_cast<uint32_t>(context.device.present_queue_index)
        };

        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.queueFamilyIndexCount = 0;
        swapchain_info.pQueueFamilyIndices = nullptr;
    }

    swapchain_info.preTransform = context.device.swapchain_support.capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = present_mode;
    swapchain_info.clipped = VK_TRUE;
    swapchain_info.oldSwapchain = nullptr;

    VK_CHECK(vkCreateSwapchainKHR(
        context.device.logical_device,
        &swapchain_info,
        context.allocator,
        &swapchain.handle
    ));

    context.current_frame = 0;

    swapchain.image_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(
        context.device.logical_device,
        swapchain.handle,
        &swapchain.image_count,
        nullptr
    ));
    if (swapchain.images.empty()) {
        swapchain.images.resize(swapchain.image_count);
    }
    if (swapchain.views.empty()) {
        swapchain.views.resize(swapchain.image_count);
    }

    VK_CHECK(vkGetSwapchainImagesKHR(
        context.device.logical_device,
        swapchain.handle,
        &swapchain.image_count,
        swapchain.images.data()
    ));

    // Create the image views
    for (uint32_t i = 0; i < swapchain.image_count; i++) {
        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = swapchain.images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = swapchain.image_format.format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(
            context.device.logical_device,
            &view_info,
            context.allocator,
            &swapchain.views[i]
        ));
    }

    if (!vkdevice_detect_depth_format(context.device)) {
        context.device.depth_format = VK_FORMAT_UNDEFINED;
        std::cout << "Failed to find a supported depth format" << std::endl;
    }

    (void)vkimage_create(
        context,
        VK_IMAGE_TYPE_2D,
        swapchain_extent.width,
        swapchain_extent.height,
        context.device.depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        swapchain.depth_attachment
    );

    std::cout << "Swapchain created..." << std::endl;
}

// Destroy the swapchain
void
destroy(VKContext& context, VKSwapchain& swapchain) {
    std::cout << "Destroying swapchain... ";
    vkDeviceWaitIdle(context.device.logical_device);
    vkimage_destroy(context, swapchain.depth_attachment);

    // Only destroy the views not images since those are owned by the swapchain
    // and are destroyed when it is
    for (uint32_t i =0 ; i < swapchain.image_count; i++) {
        vkDestroyImageView(context.device.logical_device, swapchain.views[i], context.allocator);
    }

    vkDestroySwapchainKHR(context.device.logical_device, swapchain.handle, context.allocator);
    std::cout << "Destroyed." << std::endl;
}