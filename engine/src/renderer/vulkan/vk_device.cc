#include "vulkan_backend.hh"
#include "vk_device.hh"
#include "core/qlogger.hh"

struct physical_device_requirements {
    bool graphics;
    bool present;
    bool compute;
    bool transfer;

    std::vector<const char*> device_extension_names;
    bool sampler_anisotropy;
    bool discrete_gpu;
};

struct physical_device_queue_family_info {
    uint32_t graphics_family_index;
    uint32_t present_family_index;
    uint32_t compute_family_index;
    uint32_t transfer_family_index;
};

bool select_physical_device(VKContext& context);
bool physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties& properties,
    const VkPhysicalDeviceFeatures& features,
    const physical_device_requirements& requirements,
    physical_device_queue_family_info& out_queue_family_info,
    VKSwapchainSupportInfo& out_swapchain_support
);
void device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    VKSwapchainSupportInfo& out_support_info
);

bool
VulkanBackend::create_device() {
    if (!select_physical_device(m_context)) {
        return false;
    } 

    // Create the logical device
    qlogger::Info("Creating logical device");
    // NOTE: Do not create an additional queue for shared indices
    bool present_shared_graphics_queue = m_context.device.graphics_queue_index == m_context.device.present_queue_index;
    bool transfered_shared_graphics_queue = m_context.device.graphics_queue_index == m_context.device.transfer_queue_index;
    uint32_t index_count = 1;
    if (!present_shared_graphics_queue) {
        index_count++;
    }
    if (!transfered_shared_graphics_queue) {
        index_count++;
    }

    // uint32_t indices[index_count]; -- VLA
    uint32_t indices[32];
    uint8_t index = 0;
    indices[index++] = m_context.device.graphics_queue_index;
    if (!present_shared_graphics_queue) {
        indices[index++] = m_context.device.present_queue_index;
    }
    if (!transfered_shared_graphics_queue) {
        indices[index++] = m_context.device.transfer_queue_index;
    }

    // VkDeviceQueueCreateInfo queue_create_infos[index_count];
    VkDeviceQueueCreateInfo queue_create_infos[32];
    for (uint32_t i = 0; i < index_count; i++) {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = indices[i];
        queue_create_infos[i].queueCount = 1;

        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = nullptr;
        float queue_priority = 1.0f;
        queue_create_infos[i].pQueuePriorities = &queue_priority;
    }

    // Request for device features
    VkPhysicalDeviceFeatures device_features {};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_create_info {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = index_count;
    device_create_info.pQueueCreateInfos = queue_create_infos;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = 1;
    const char* extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    device_create_info.ppEnabledExtensionNames = &extension_names;

    // Deprecated and ignored
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = nullptr;

    // Create the device
    VK_CHECK(vkCreateDevice(
        m_context.device.physical_device,
        &device_create_info,
        m_context.allocator,
        &m_context.device.logical_device
    ));

    qlogger::Info("Logical Device created...");

    // Get the device queues
    vkGetDeviceQueue(
        m_context.device.logical_device,
        m_context.device.graphics_queue_index,
        0,
        &m_context.device.graphics_queue
    );


    vkGetDeviceQueue(
        m_context.device.logical_device,
        m_context.device.present_queue_index,
        0,
        &m_context.device.present_queue
    );
    
    vkGetDeviceQueue(
        m_context.device.logical_device,
        m_context.device.transfer_queue_index,
        0,
        &m_context.device.transfer_queue
    );
    qlogger::Info("Queues obtained...");

    // Create the command pool for the graphcis queue
    VkCommandPoolCreateInfo pool_create_info {};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_create_info.queueFamilyIndex = m_context.device.graphics_queue_index;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(
        m_context.device.logical_device,
        &pool_create_info,
        m_context.allocator,
        &m_context.device.graphics_command_pool
    ));
    qlogger::Info("Graphics command pool created...");

    return true;
}


bool 
select_physical_device(VKContext& context) {
    // Evaluate device properties
    uint32_t physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, nullptr));
    if (physical_device_count == 0) {
        qlogger::Info("No devices which support vulkan were found");
        return false;
    }

    // VkPhysicalDevice physical_devices[physical_device_count];
    constexpr uint32_t MAX_DEVICE_COUNT = 32;
    VkPhysicalDevice physical_devices[MAX_DEVICE_COUNT];
    VK_CHECK(vkEnumeratePhysicalDevices(context.instance, &physical_device_count, physical_devices));

    for (uint32_t i = 0; i < physical_device_count; i++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        // TODO: This should be driven by the engine
        physical_device_requirements requirements {};
        requirements.graphics = true;
        requirements.present = true;
        requirements.transfer = true;

        requirements.sampler_anisotropy = true;
        requirements.discrete_gpu = true;
        requirements.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        physical_device_queue_family_info queue_family_info {};
        bool result = physical_device_meets_requirements(
            physical_devices[i],
            context.surface,
            properties,
            features,
            requirements,
            queue_family_info,
            context.device.swapchain_support
        );

        if (result) {
            qlogger::Info("Selected device %s", properties.deviceName);
            switch(properties.deviceType) {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    qlogger::Info("GPU Type is unknown");
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    qlogger::Info("GPU Type is Integrated");
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    qlogger::Info("GPU Type is Discrete");
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    qlogger::Info("GPU Type is Virtual");
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    qlogger::Info("GPU Type is CPU");
            }

            qlogger::Info("GPU Driver Version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.driverVersion),
                VK_VERSION_MINOR(properties.driverVersion),
                VK_VERSION_PATCH(properties.driverVersion));
            
            qlogger::Info("GPU API Version: %d.%d.%d",
                VK_VERSION_MAJOR(properties.apiVersion),
                VK_VERSION_MINOR(properties.apiVersion),
                VK_VERSION_PATCH(properties.apiVersion));

            // Memory version
            for (uint32_t j = 0; j < memory.memoryHeapCount; j++) {
                float memory_size_gib = (((float)memory.memoryHeaps[j].size) / 1024.0F / 1024.0F / 1024.0F);
                if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    qlogger::Info("Local GPU memory: %.2f GiB", memory_size_gib);
                } else {
                    qlogger::Info("Shared System memory: %.2f", memory_size_gib);
                }
            }

            context.device.physical_device = physical_devices[i];
            context.device.graphics_queue_index = queue_family_info.graphics_family_index;
            context.device.present_queue_index = queue_family_info.present_family_index;
            context.device.transfer_queue_index = queue_family_info.transfer_family_index;

            context.device.properties = properties;
            context.device.features = features;
            context.device.memory = memory;
            break;
        }
    }

    // Make sure we selected a device
    if (!context.device.physical_device) {
        qlogger::Info("ERROR: Failed to select a physical device that meets the requirements");
        return false;
    }

    qlogger::Info("Physical device selected.");
    return true;
}


void
vkdevice_query_swapchain_support(VKDevice& device, VkSurfaceKHR surface, VKSwapchainSupportInfo& out_support_info) {
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device.physical_device,
        surface,
        &out_support_info.capabilities
    ));

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        device.physical_device,
        surface,
        &out_support_info.format_count,
        nullptr
    ));

    if (out_support_info.format_count != 0) {
        if (out_support_info.formats.empty()) {
            out_support_info.formats.resize(out_support_info.format_count);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            device.physical_device,
            surface,
            &out_support_info.format_count,
            out_support_info.formats.data()
        ));
    }

    // Present modes
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        device.physical_device,
        surface,
        &out_support_info.present_mode_count,
        nullptr
    ));
    if (out_support_info.present_mode_count != 0) {
        if (out_support_info.present_modes.empty()) {
            out_support_info.present_modes.resize(out_support_info.present_mode_count);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            device.physical_device,
            surface,
            &out_support_info.present_mode_count,
            out_support_info.present_modes.data()
        ));
    }
}

bool 
vkdevice_detect_depth_format(
    VKDevice& device
) {
    const uint64_t candidate_count = 3;
    std::array<VkFormat, 3> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    uint32_t flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for (uint64_t i = 0; i < candidate_count; i++) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(device.physical_device, candidates[i], &properties);

        if ((properties.linearTilingFeatures & flags) == flags) {
            device.depth_format = candidates[i];
            return true;
        } else if ((properties.optimalTilingFeatures & flags) == flags) {
            device.depth_format = candidates[i];
            return true;
        }
    }

    return false;
}

void 
VulkanBackend::destroy_device() {
    // Unset the queues
    m_context.device.graphics_queue = nullptr;
    m_context.device.present_queue = nullptr;
    m_context.device.transfer_queue = nullptr;

    qlogger::Info("Destroying command pools... ");
    vkDestroyCommandPool(
        m_context.device.logical_device,
        m_context.device.graphics_command_pool,
        m_context.allocator
    );
    qlogger::Info("Destroyed.");

    qlogger::Info("Destroying the logical device... ");
    if (m_context.device.logical_device) {
        vkDestroyDevice(m_context.device.logical_device, m_context.allocator);
        m_context.device.logical_device = nullptr;
    }
    qlogger::Info("Destroyed.");

    qlogger::Info("Releasing physical device resources... ");
    m_context.device.physical_device = nullptr;

    m_context.device.graphics_queue_index = -1;
    m_context.device.present_queue_index = -1;
    m_context.device.transfer_queue_index = -1;
    qlogger::Info("Released.");
}

void 
device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    VKSwapchainSupportInfo& out_support_info
) {
    // Surface capabilities
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device,
        surface,
        &out_support_info.capabilities
    ));
    
    // Surface formats
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        physical_device,
        surface,
        &out_support_info.format_count,
        nullptr
    ));

    if (out_support_info.format_count != 0) {
        if (out_support_info.formats.empty()) {
            out_support_info.formats.resize(out_support_info.format_count);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device,
            surface,
            &out_support_info.format_count,
            out_support_info.formats.data()
        ));
    }

    // Present mode
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device,
        surface,
        &out_support_info.present_mode_count,
        nullptr
    ));
    if (out_support_info.present_mode_count != 0) {
        if (out_support_info.present_modes.empty()) {
            out_support_info.present_modes.resize(out_support_info.present_mode_count);
        }

        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device,
            surface,
            &out_support_info.present_mode_count,
            out_support_info.present_modes.data()
        ));
    }
}

bool 
physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties& properties,
    const VkPhysicalDeviceFeatures& features,
    const physical_device_requirements& requirements,
    physical_device_queue_family_info& out_queue_family_info,
    VKSwapchainSupportInfo& out_swapchain_support
) {
    out_queue_family_info.graphics_family_index = -1;
    out_queue_family_info.present_family_index = -1;
    out_queue_family_info.compute_family_index = -1;
    out_queue_family_info.transfer_family_index = -1;

    // Discrete GPU?
    if (requirements.discrete_gpu) {
        if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            qlogger::Info("Device is not a discrete GPU, and one is required. Skipping...");
            return false;
        }
    }

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    // VkQueueFamilyProperties queue_families[queue_family_count]; -- VLA
    VkQueueFamilyProperties queue_families[32];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    qlogger::Info("Graphics | Present | Compute | Transfer | Name");
    uint8_t min_transfer_score = 255;
    for (uint32_t i = 0; i < queue_family_count; i++) {
        uint8_t current_transfer_score = 0;

        // Graphics queue?
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            out_queue_family_info.graphics_family_index = i;
            current_transfer_score++;
        }

        // Compute Queue?
        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            out_queue_family_info.compute_family_index = i;
            current_transfer_score++;
        }

        //Transfer Queue?
        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            // Take the index if it is the current lowest. This 
            // increases the likelihood that is is a dedicated transfer queue
            if (current_transfer_score <= min_transfer_score) {
                min_transfer_score = current_transfer_score;
                out_queue_family_info.transfer_family_index = i;
            }
        }

        // Present Queue?
        VkBool32 supports_present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present));
        if (supports_present == VK_TRUE) {
            out_queue_family_info.present_family_index = i;
        }
    }

    qlogger::Info("        %d |        %d |        %d |        %d | %s\n",
        out_queue_family_info.graphics_family_index != -1,
        out_queue_family_info.present_family_index != -1,
        out_queue_family_info.compute_family_index != -1,
        out_queue_family_info.transfer_family_index != -1,
        properties.deviceName
    );

    // Query for swapchain support
    device_query_swapchain_support(
        device,
        surface,
        out_swapchain_support
    );

    if (out_swapchain_support.format_count < 1 || out_swapchain_support.present_mode_count < 1) {
        if (out_swapchain_support.formats.size() > 0) {
            out_swapchain_support.formats.clear();
        }

        if (out_swapchain_support.present_modes.size() > 0) {
            out_swapchain_support.present_modes.clear();
        }
        qlogger::Debug("Required swapchain support not present, skipping device.");
        return false;
    }

    // Device extensions
    if  (requirements.device_extension_names.size() > 0) {
        uint32_t available_extension_count = 0;
        std::vector<VkExtensionProperties> available_extensions;
        VK_CHECK(
            vkEnumerateDeviceExtensionProperties(
                device, NULL,
                &available_extension_count,
                nullptr
            )
        );
        available_extensions.resize(available_extension_count);
        VK_CHECK(
            vkEnumerateDeviceExtensionProperties(
                device, NULL,
                &available_extension_count,
                available_extensions.data()
            )
        );

        if (available_extension_count != 0) {
            for (size_t i = 0; i < requirements.device_extension_names.size(); i++) {
                bool found = false;
                for (size_t j = 0; j < available_extensions.size(); j++) {
                    if (strcmp(requirements.device_extension_names[i], available_extensions[j].extensionName) == 0) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    qlogger::Info("Required extension not found: '%s'. Skipping device...\n", requirements.device_extension_names[i]);
                    return false;
                }
            }
        }

        // Sampler anisotropy
        if (requirements.sampler_anisotropy && !features.samplerAnisotropy) {
            qlogger::Info("Device does not support sampler anisotropy. Skipping...");
            return false;
        }

        // Device meets all requirements
        return true;
    }

    return false;
}