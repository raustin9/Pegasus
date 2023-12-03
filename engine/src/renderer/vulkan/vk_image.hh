#pragma once
#include "vulkan_types.hh"

void vkimage_create(
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
);

void vkimage_view_create(
    VKContext& context,
    VkFormat format,
    VKImage& image,
    VkImageAspectFlags aspect_flags
);

void vkimage_destroy(VKContext& context, VKImage& image);