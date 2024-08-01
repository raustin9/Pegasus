#include "vulkan_types.hh"
#include "vk_image.hh"
#include "vulkan_utils.hh"
#include "core/qmemory.hh"
#include "core/qlogger.hh"


void 
VKImage::create(
    VKContext& context,
    VkImageType image_type,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memory_flags,
    bool create_view,
    VkImageAspectFlags view_aspect_flags
) {
    this->context = &context;
    this->width = width;
    this->height = height;

    // Creation info
    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;     // TODO: Support configurable depth
    image_info.mipLevels = 4;        // TODO: Support mip mapping
    image_info.arrayLayers = 1; // TODO: Support number of layers in the image
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;         // TODO: Configurable sample count
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode

    VK_CHECK(vkCreateImage(
        context.device.logical_device,
        &image_info,
        context.allocator,
        &this->handle
    ));

    // Query the memory requirements for this image
    VkMemoryRequirements memreqs;
    vkGetImageMemoryRequirements(
        context.device.logical_device,
        this->handle,
        &memreqs
    );

    int32_t mem_type = context.find_memory_index(
        memreqs.memoryTypeBits,
        memory_flags
    );

    if (mem_type == -1) {
        qlogger::Error("Error: Required memory type not found. Image is not valid.");
    }

    // Allocate memory for the image
    VkMemoryAllocateInfo mem_alloc_info = {};
    mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc_info.allocationSize = memreqs.size;
    mem_alloc_info.memoryTypeIndex = mem_type;

    VK_CHECK(vkAllocateMemory(
        context.device.logical_device,
        &mem_alloc_info,
        context.allocator,
        &this->memory
    ));

    // Bind the memory
    VK_CHECK(vkBindImageMemory(
        context.device.logical_device,
        this->handle,
        this->memory,
        0
    )); // TODO: Configurable memory offset

    if (create_view) {
        this->view = nullptr;
        vkimage_view_create(context, format, *this, view_aspect_flags);
    }
}


void
VKImage::view_create(VkFormat format, VkImageAspectFlags flags) {
    
}

void 
vkimage_create(
    VKContext& context,
    VkImageType image_type,
    uint32_t width,
    uint32_t height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memory_flags,
    bool create_view,
    VkImageAspectFlags view_aspect_flags,
    VKImage& out_image
) {
    out_image.width = width;
    out_image.height = height;

    // Creation info
    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;     // TODO: Support configurable depth
    image_info.mipLevels = 4;        // TODO: Support mip mapping
    image_info.arrayLayers = 1; // TODO: Support number of layers in the image
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;         // TODO: Configurable sample count
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Configurable sharing mode

    VK_CHECK(vkCreateImage(
        context.device.logical_device,
        &image_info,
        context.allocator,
        &out_image.handle
    ));

    // Query the memory requirements for this image
    VkMemoryRequirements memreqs;
    vkGetImageMemoryRequirements(
        context.device.logical_device,
        out_image.handle,
        &memreqs
    );

    int32_t mem_type = context.find_memory_index(
        memreqs.memoryTypeBits,
        memory_flags
    );

    if (mem_type == -1) {
        qlogger::Error("Required memory type not found. Image is not valid");
    }

    // Allocate memory for the image
    VkMemoryAllocateInfo mem_alloc_info = {};
    mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc_info.allocationSize = memreqs.size;
    mem_alloc_info.memoryTypeIndex = mem_type;

    VK_CHECK(vkAllocateMemory(
        context.device.logical_device,
        &mem_alloc_info,
        context.allocator,
        &out_image.memory
    ));

    // Bind the memory
    VK_CHECK(vkBindImageMemory(
        context.device.logical_device,
        out_image.handle,
        out_image.memory,
        0
    )); // TODO: Configurable memory offset

    if (create_view) {
        out_image.view = nullptr;
        vkimage_view_create(context, format, out_image, view_aspect_flags);
    }
}

void 
vkimage_view_create(
    VKContext& context,
    VkFormat format,
    VKImage& image,
    VkImageAspectFlags aspect_flags
) {
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image.handle;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = format;
    view_info.subresourceRange.aspectMask = aspect_flags;

    // TODO: Make configrurable
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(
        context.device.logical_device,
        &view_info,
        context.allocator,
        &image.view
    ));
}

void
vkimage_destroy(VKContext& context, VKImage& image) {
    if (image.view) {
        vkDestroyImageView(context.device.logical_device, image.view, context.allocator);
    }

    if (image.memory) {
        vkFreeMemory(context.device.logical_device, image.memory, context.allocator);
    }

    if (image.handle) {
        vkDestroyImage(context.device.logical_device, image.handle, context.allocator);
    }
}

// Destruction behavior for VKImage
void
VKImage::destroy(VKContext& context) {
    if (this->view) {
        vkDestroyImageView(context.device.logical_device, this->view, context.allocator);
    }

    if (this->memory) {
        vkFreeMemory(context.device.logical_device, this->memory, context.allocator);
    }

    if (this->handle) {
        vkDestroyImage(context.device.logical_device, this->handle, context.allocator);
    }
}

/**
    * Transitions the provided image from old_layout to new_layout
*/
void 
VKImage::transition_layout(
    // VKContext& context,
    VKCommandBuffer& command_buffer,
    VkFormat format,
    VkImageLayout old_layout,
    VkImageLayout new_layout
) {
    VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = this->context->device.graphics_queue_index;
    barrier.dstQueueFamilyIndex = this->context->device.graphics_queue_index;
    barrier.image = this->handle;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags source_stage;
    VkPipelineStageFlags dest_stage;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        // Dont care what stage the pipeline is in at the start
        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

        // Used for copying
        dest_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        // Transitioning from a transfer destination layout to a shader readonly layout
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        // From a copying stage to...
        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        // The fragment stage
        dest_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        qlogger::Fatal("VKImage::transition_layout: unsupported layout transition");
        return;
    }

    vkCmdPipelineBarrier(
        command_buffer.handle,
        source_stage,
        dest_stage,
        0,
        0,
        0,
        0,
        0,
        1,
        &barrier
    );
}

/**
    * Copies the data in from the buffer to this image
    * @param context The vulkan context
    * @param image The image to copy the buffer's data to
    * @param buffer The buffer whose data will be copied
*/
void 
VKImage::copy_from_buffer(
    // VKContext& context,
    VkBuffer buffer,
    VKCommandBuffer& command_buffer
) {
    // Region to copy
    VkBufferImageCopy region {};
    QAllocator::Zero(&region, sizeof(VkBufferImageCopy));
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.layerCount = 1;

    region.imageExtent.width = this->width;
    region.imageExtent.height = this->height;
    region.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(
        command_buffer.handle,
        buffer,
        this->handle,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
}