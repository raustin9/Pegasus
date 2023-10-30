#include "renderer.hh"
#include "core/application.hh"
#include <vulkan/vulkan_core.h>

Renderer::Renderer(std::string name, uint32_t width, uint32_t height)
    : m_title(name), 
    m_width(width), 
    m_height(height),
    m_platform{name, width, height}
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


void
Renderer::OnDestroy() {
}

void 
Renderer::InitVulkan() {
    CreateInstance();
}


void 
Renderer::CreateInstance() {
    VkApplicationInfo appinfo = {};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pApplicationName = GetTitle();
    appinfo.pEngineName = GetTitle();
    appinfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> instanceExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

    // TODO: platform specific ext names

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
        createInfo.enabledLayerCount = static_cast<uint32_t>(instanceExtensions.size());
        createInfo.ppEnabledExtensionNames = instanceExtensions.data();
    }
}
