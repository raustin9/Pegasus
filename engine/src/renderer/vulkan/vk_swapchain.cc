#include "vulkan_backend.hh"
#include "vk_device.hh"
#include "vk_image.hh"
#include "core/qlogger.hh"

// VULKAN BACKEND METHODS //
bool 
VulkanBackend::create_swapchain(uint32_t width, uint32_t height, VKSwapchain& out_swapchain) {
    // Create a new one
    m_context.swapchain.create(m_context, width, height);
    // create(m_context, width, height, out_swapchain);
    return true;
}

void
VulkanBackend::destroy_swapchain() {
    m_context.swapchain.destroy(m_context);
    // destroy(m_context, m_context.swapchain);
}



// VULKAN SWAPCHAIN METHODS //
bool
VKSwapchain::recreate(VKContext& context, uint32_t width, uint32_t height) {
    // Destroy old swapchain and crete a new one
    // destroy(context, &this);
    // create(context, width, height, &this);
    this->destroy(context);
    this->create(context, width, height);
    return true;
}

void
VKSwapchain::create(VKContext& context, uint32_t width, uint32_t height) {
    VkExtent2D swapchain_extent = {width, height};
    // this->max_frames_in_flight = 2;

    bool found = false;
    for (uint32_t i = 0; i < context.device.swapchain_support.format_count; i++) {
        VkSurfaceFormatKHR format = context.device.swapchain_support.formats[i];
        // Preferred formats
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM 
            && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            this->image_format = format;
            found = true;
            break;
        }
    }

    if (!found) {
        this->image_format = context.device.swapchain_support.formats[0];
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
    this->max_frames_in_flight = image_count-1;

    VkSwapchainCreateInfoKHR swapchain_info {};
    swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_info.surface = context.surface;
    swapchain_info.minImageCount = image_count;
    swapchain_info.imageFormat = this->image_format.format;
    swapchain_info.imageColorSpace = this->image_format.colorSpace;
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
        &this->handle
    ));

    context.current_frame = 0;

    this->image_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(
        context.device.logical_device,
        this->handle,
        &this->image_count,
        nullptr
    ));
    if (this->images.empty()) {
        this->images.resize(this->image_count);
    }
    if (this->views.empty()) {
        this->views.resize(this->image_count);
    }

    VK_CHECK(vkGetSwapchainImagesKHR(
        context.device.logical_device,
        this->handle,
        &this->image_count,
        this->images.data()
    ));

    // Create the image views
    for (uint32_t i = 0; i < this->image_count; i++) {
        VkImageViewCreateInfo view_info {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = this->images[i];
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = this->image_format.format;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(
            context.device.logical_device,
            &view_info,
            context.allocator,
            &this->views[i]
        ));
    }

    if (!vkdevice_detect_depth_format(context.device)) {
        context.device.depth_format = VK_FORMAT_UNDEFINED;
        qlogger::Info("Failed to find a supported depth format");
    }

    // (void)vkimage_create(
    //     context,
    //     VK_IMAGE_TYPE_2D,
    //     swapchain_extent.width,
    //     swapchain_extent.height,
    //     context.device.depth_format,
    //     VK_IMAGE_TILING_OPTIMAL,
    //     VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
    //     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    //     true,
    //     VK_IMAGE_ASPECT_DEPTH_BIT,
    //     this->depth_attachment
    // );
    // TODO: make sure that this implementation works properly
    this->depth_attachment.create(
        context,
        VK_IMAGE_TYPE_2D,
        swapchain_extent.width,
        swapchain_extent.height,
        context.device.depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true,
        VK_IMAGE_ASPECT_DEPTH_BIT
    );

    qlogger::Info("Swapchain created...");
}

// Destroy the swapchain
void
VKSwapchain::destroy(VKContext& context) {
    qlogger::Info("Destroying swapchain... ");
    vkDeviceWaitIdle(context.device.logical_device);
    // vkimage_destroy(context, this->depth_attachment);
    this->depth_attachment.destroy(context);

    // Only destroy the views not images since those are owned by the swapchain
    // and are destroyed when it is
    for (uint32_t i =0 ; i < this->image_count; i++) {
        vkDestroyImageView(context.device.logical_device, this->views[i], context.allocator);
    }

    vkDestroySwapchainKHR(context.device.logical_device, this->handle, context.allocator);
    qlogger::Info("Destroyed.");
}

bool
VKSwapchain::acquire_next_image_index(
    VKContext& context, 
    uint64_t timeout_ms, 
    VkSemaphore image_available_semaphore, 
    VkFence fence, 
    uint32_t& out_image_index
) {
    VkResult result = vkAcquireNextImageKHR(
        context.device.logical_device,
        this->handle,
        timeout_ms,
        image_available_semaphore,
        fence,
        &out_image_index
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        this->recreate(context, context.framebuffer_width, context.framebuffer_height);
        return false;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        qlogger::Info("Error: Failed to acquire swapchain image");
        return false;
    }

    return true;
}

bool
VKSwapchain::present(
    VKContext& context,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete_semaphore,
    uint32_t present_image_index
) {
    VkPresentInfoKHR present_info {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_complete_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &this->handle;
    present_info.pImageIndices = &present_image_index;
    present_info.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(
        present_queue,
        &present_info
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // swapchain is out of date or framebuffer resize has occrured
        this->recreate(context, context.framebuffer_width, context.framebuffer_height);
        return false;
    } else if (result != VK_SUCCESS) {
        qlogger::Info("Error: Failed to present swapchain image");
        return false;
    }

    context.current_frame = (context.current_frame + 1) % this->max_frames_in_flight;
    return true;
}