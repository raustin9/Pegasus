#include "vulkan_types.hh"
#include <iostream>

int32_t 
VKContext::find_memory_index(uint32_t type_filter, uint32_t property_flags) {
    VkPhysicalDeviceMemoryProperties mem_properties = {};
    vkGetPhysicalDeviceMemoryProperties(device.physical_device, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if (type_filter & (1 << i) && (mem_properties.memoryTypes[i].propertyFlags) == property_flags) {
            return i;
        }
    }

    std::cout << "WARNING: Unable to find suitable memory type" << std::endl;
    return -1;
}