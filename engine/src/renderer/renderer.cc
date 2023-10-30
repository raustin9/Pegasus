#include "renderer.hh"
#include "core/application.hh"
#include "renderer/vkcommon.hh"
#include "renderer/debug.hh"
#include "renderer/vkdevice.hh"

#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include <stdlib.h>

Renderer::Renderer(std::string name, uint32_t width, uint32_t height, Platform& platform)
    : m_title(name), 
    m_width(width), 
    m_height(height),
    m_platform(platform)
{
    m_aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
    m_vkparams.Allocator = nullptr;
}

void
Renderer::OnInit() {
    InitVulkan();
}

void
Renderer::OnUpdate() {
}

void
Renderer::OnRender() {
}


// Destroy in reverse order of creation
void
Renderer::OnDestroy() {
    DestroySurface();
    DestroyInstance();
}

void 
Renderer::InitVulkan() {
    CreateInstance();
    CreateSurface();
    CreateDevice();
}


void 
Renderer::CreateInstance() {
    VkApplicationInfo appinfo = {};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pApplicationName = GetTitle();
    appinfo.pEngineName = GetTitle();
    appinfo.apiVersion = VK_API_VERSION_1_2;

    std::vector<const char*> instanceExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

    // TODO: platform specific ext names
#if defined(Q_PLATFORM_LINUX)
    instanceExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined (Q_PLATFORM_WINDOWS)
    instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    // Validation layer ext
    if (Application::settings.enableValidation) {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // Get the supported extensions
    std::cout << "Supported extensions:" << std::endl;
    uint32_t extensionCount = 0;
    std::vector<std::string> extensionNames;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (extensionCount > 0) {
        std::vector<VkExtensionProperties> supportedExtensions(extensionCount);

        if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, &supportedExtensions.front()) == VK_SUCCESS) {
            for (size_t i = 0; i < supportedExtensions.size(); i++) {
                std::cout << "\t" << supportedExtensions[i].extensionName << std::endl;
                extensionNames.push_back(supportedExtensions[i].extensionName);
            }
        } else {
            printf("vkEnumerateInstanceExtensionProperties did not return VK_SUCCESS\n");
            exit(1);
        }
    }

    // Create the vulkan instance
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.pApplicationInfo = &appinfo;

    // Print out required extensions
    std::cout << "Required extensions:" << std::endl;

    // Check that the extensions we need are supported
    if (instanceExtensions.size() > 0) {
        for (size_t i = 0; i < instanceExtensions.size(); i++) {
            std::cout << "\t" << instanceExtensions[i] << std::endl;
            // Output if requested ext is not available
            if (std::find(extensionNames.begin(), extensionNames.end(), instanceExtensions[i]) == extensionNames.end()) {
                printf("Extension %s not found\n", instanceExtensions[i]);
                exit(1);
            }
        }

        // set extension to enable
        createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        createInfo.ppEnabledExtensionNames = instanceExtensions.data();
    }

    // Validation layer setup
    const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
    if (Application::settings.enableValidation) {
        // Check if this layer is available at instance level
        uint32_t instanceLayerCount;
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
        std::vector <VkLayerProperties> instanceLayerProps(instanceLayerCount);
        vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProps.data());
        bool validationPresent = false;

        for (size_t i = 0; i < instanceLayerProps.size(); i++) {
            if (strcmp(instanceLayerProps[i].layerName, validationLayerName) == 0) {
                validationPresent = true;
                break;
            }
        }

        if (validationPresent) {
            createInfo.enabledLayerCount = 1;
            createInfo.ppEnabledLayerNames = &validationLayerName;
        } else {
            std::cout << "Validation layer VK_LAYER_KHRONOS_validation not present. Validation is disabled" << std::endl;
            exit(1);
        }

        VK_CHECK(vkCreateInstance(&createInfo, m_vkparams.Allocator, &m_vkparams.Instance));

        // Set callback to handle validation
        if (Application::settings.enableValidation)
            setupDebugUtil(m_vkparams.Instance);
    }
}

// Create the window surface
void
Renderer::CreateSurface() {
    // Use platform-specific surface creation function
    m_platform.create_vulkan_surface(m_vkparams);
    std::cout << "Surface created" << std::endl;
}

void
Renderer::CreateDevice() {
    // Get the physical device
    uint32_t gpuCount = 0;

    // Get number of physical devices
    vkEnumeratePhysicalDevices(m_vkparams.Instance, &gpuCount, nullptr);
    if (gpuCount == 0) {
        std::cout << "No device with Vulkan support found" << std::endl;
        exit(1);
    }

    // Enumerate the devices
    std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
    VkResult err = vkEnumeratePhysicalDevices(m_vkparams.Instance, &gpuCount, &physicalDevices.front());
    if (err) {
        throw std::runtime_error("Could not enumerate physical devices\n");
    }

    // Select physical device that has a graphics queue
    for (size_t i = 0; i < gpuCount; i++) {
        if (CheckPhysicalDeviceProperties(physicalDevices[i], m_vkparams)) {
            m_vkparams.Device.PhysicalDevice = physicalDevices[i];
            vkGetPhysicalDeviceProperties(m_vkparams.Device.PhysicalDevice, &m_deviceProperties);
            break;
        }
    }
    std::cout << "GOT HERE" <<std::endl;
   
    // Make sure we have a valid physical device
    if (m_vkparams.Device.PhysicalDevice == VK_NULL_HANDLE
        || m_vkparams.GraphicsQueue.FamilyIndex == UINT32_MAX) {
        throw std::runtime_error("Could not select physical device based on chosen properties\n");
    } else {
        vkGetPhysicalDeviceFeatures(m_vkparams.Device.PhysicalDevice, &m_vkparams.Device.DeviceFeatures);
        vkGetPhysicalDeviceMemoryProperties(m_vkparams.Device.PhysicalDevice, &m_vkparams.Device.DeviceMemoryProperties);
    }

    // Desired queues need to be requested upon logical devices
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

    // Array of normalized vector floating point values (between 0 and 1 inclusive)
    // specifying priotities of work to each requested queue.
    // Higher vals mean higher prio with 0.0 being the lowest
    // Within the same device, queues with higher prio may be allotted more 
    // processing time than queues with lower prio
    const float queuePriorities[] = {1.0f};

    // Request a single graphics queue
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = m_vkparams.GraphicsQueue.FamilyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = queuePriorities;
    queueCreateInfos.push_back(queueInfo);

    // Add swapchain extension
    std::vector <const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Get the list of supported device extensions
    uint32_t extCount = 0;
    std::vector <std::string> supportedDeviceExtensions;
    vkEnumerateDeviceExtensionProperties(m_vkparams.Device.PhysicalDevice, nullptr, &extCount, nullptr);
    if (extCount > 0) {
        std::vector <VkExtensionProperties> extensions(extCount);
        if (vkEnumerateDeviceExtensionProperties(m_vkparams.Device.PhysicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS) {
            for (size_t i = 0; i < extensions.size(); i++) {
                supportedDeviceExtensions.push_back(extensions[i].extensionName);
            }
        }
    }


    // Create the logical device
    if (CreateLogicalDevice(queueCreateInfos, deviceExtensions, supportedDeviceExtensions, m_vkparams) != VK_SUCCESS) {
        throw std::runtime_error("CreateLogicalDevice() could not create vulkan logical device");
    }

    std::cout << "Device created" << std::endl;
}


//
// Destroy Vulkan items
//


void
Renderer::DestroyInstance() {
    // TODO: destroy vulkan instance
}


void
Renderer::DestroySurface() {
    // TODO: destroy vulkan surface 
}
