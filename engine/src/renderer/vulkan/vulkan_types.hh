#pragma once
#include "defines.hh"
#include <vulkan/vulkan.h>
#include <vector>

struct VKSwapchainSupportInfo {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t format_count;
    std::vector<VkSurfaceFormatKHR> formats;
    uint32_t present_mode_count;
    std::vector<VkPresentModeKHR> present_modes;
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
};