#pragma once
#include "defines.hh"
#include <vulkan/vulkan.h>
#include <vector>

struct VKSwapchainSupportInfo {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t format_count;
    std::vector<VkSurfaceFormatKHR> formats{};
    uint32_t present_mode_count;
    std::vector<VkPresentModeKHR> present_modes{};
};

// Information for the logical and physical devices
struct VKDevice {
    VkPhysicalDevice       physical_device;
    VkDevice               logical_device;
    VKSwapchainSupportInfo swapchain_support;

    int32_t graphics_queue_index;
    int32_t present_queue_index;
    int32_t transfer_queue_index;
    // NOTE: put compute index here if needed

    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;

    VkCommandPool graphics_command_pool;

    VkPhysicalDeviceProperties       properties;
    VkPhysicalDeviceFeatures         features;
    VkPhysicalDeviceMemoryProperties memory;

    VkFormat depth_format;
};

struct VKImage {
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    uint32_t width;
    uint32_t height;
};

struct VKRenderpass {
    VkRenderPass handle;
    float x, y, w, h;
    float r, g, b, a;

    float depth;
    float stencil;
};

struct VKFramebuffer {
    VkFramebuffer handle;
    uint32_t attachment_count;
    std::vector<VkImageView> attachments;
    VKRenderpass *renderpass;
};

struct VKSwapchain {

    VkSurfaceFormatKHR image_format;
    uint8_t max_frames_in_flight;
    VkSwapchainKHR handle;
    uint32_t image_count;
    std::vector<VkImage> images;
    std::vector<VkImageView> views;

    VKImage depth_attachment;
    std::vector<VKFramebuffer> framebuffers;
};

// Holds vulkan-specific information
// for the renderer backend
struct VKContext {
    uint32_t image_index;
    uint32_t current_frame;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint32_t framebuffer_size_generation;
    uint64_t framebuffer_size_last_generation;

    VkInstance             instance;
    VkAllocationCallbacks *allocator;
    VkSurfaceKHR           surface;
    
#if defined(P_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif // P_DEBUG

    VKDevice device;
    VKSwapchain swapchain;
    VKRenderpass main_renderpass;

    int32_t find_memory_index(uint32_t type_filter, uint32_t property_flags);
};