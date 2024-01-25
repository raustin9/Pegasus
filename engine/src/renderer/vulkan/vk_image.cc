#include "vulkan_types.hh"
#include "vk_image.hh"
#include "vulkan_utils.hh"


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
        std::cout << "Error: Required memory type not found. Image is not valid." << std::endl;
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
        std::cout << "Error: Required memory type not found. Image is not valid." << std::endl;
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