#include "vkdevice.hh"
#include "vkcommon.hh"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

bool 
CheckPhysicalDeviceProperties(const VkPhysicalDevice &physicalDevice, VKCommonParameters &params) {
    // Get list of supported device extensions
    uint32_t extCount = 0;
    std::vector<std::string> extensionNames;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, nullptr);
    if (extCount > 0) {
        std::vector <VkExtensionProperties> extensions(extCount);
        if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extCount, extensions.data()) == VK_SUCCESS) {
            for (size_t i = 0; i < extensions.size(); i++) {
                extensionNames.push_back(extensions[i].extensionName);
            }
        }
    }

    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Check that the device extensions that we want are supported
    if (deviceExtensions.size() > 0) {
        for (size_t i = 0; i < deviceExtensions.size(); i++) {
            // Output message if requested extension is not available
            if (std::find(extensionNames.begin(), extensionNames.end(), deviceExtensions[i]) == extensionNames.end()) {
                std::cerr << "Device extension not supported: " << deviceExtensions[i] << std::endl;
                exit(1);
            }
        }
    }

    // Get device queue family properties
    unsigned int queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0) {
        std::cerr << "Physical device does not have any queue families" << std::endl;
        exit(1);
    }

    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    std::vector<VkBool32> queuePresentSupport(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        std::cout << "GOT HERE\n";
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i , params.PresentationSurface, &queuePresentSupport[i]);
        if ( (queueFamilyProperties[i].queueCount > 0) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            // If the queue fam supports both graphics and operations and presentation on our surface
            // then we prefer it
            if (queuePresentSupport[i]) {
                params.GraphicsQueue.FamilyIndex = i;
                return true;
            }
        }
    }

    return false;
}

void 
GetDeviceQueue(const VkDevice &device, uint32_t graphicsQueueFamilyIndex, VkQueue& graphicsQueue) {
    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
}

// Create a vulkan logical device from the provided information
VkResult 
CreateLogicalDevice(
    std::vector<VkDeviceQueueCreateInfo> &queueInfos,
    std::vector<const char*> &deviceExtensions,
    std::vector<std::string> &supportedDeviceExtensions,
    VKCommonParameters &params
) {
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
    createInfo.pQueueCreateInfos = queueInfos.data();

    // Check that the device extensions we want to enable are supported
    if (deviceExtensions.size() > 0) {
        for (size_t i = 0; i < deviceExtensions.size(); i++) {
            // Output message if requested extension is not available
            if (std::find(supportedDeviceExtensions.begin(), supportedDeviceExtensions.end(), deviceExtensions[i]) == supportedDeviceExtensions.end()) {
                std::cerr << "Device extension: " << deviceExtensions[i] << " not supported" << std::endl;
                exit(1);
            }
        }

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    }

    return vkCreateDevice(params.Device.PhysicalDevice, &createInfo, params.Allocator, &params.Device.Device);
}
