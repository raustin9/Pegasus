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
};

// Holds vulkan-specific information
// for the renderer backend
struct VKContext {
    VkInstance             instance;
    VkAllocationCallbacks *allocator;
    VkSurfaceKHR           surface;
    
#if defined(P_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif // P_DEBUG

    VKDevice device;
};