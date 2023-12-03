#pragma once
#include "vulkan_backend.hh"


void vkdevice_query_swapchain_support(
    VKDevice& device, 
    VkSurfaceKHR surface, 
    VKSwapchainSupportInfo& out_support_info
);

bool vkdevice_detect_depth_format(
    VKDevice& device
);