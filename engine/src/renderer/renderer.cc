/*

#include "renderer.hh"
#include "core/vkapplication.hh"
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>

// Constructor
Renderer::Renderer(std::string name, uint32_t width, uint32_t height) 
    : m_title(name), 
      m_width(width), 
      m_height(height), 
      m_initialized(false),
      m_platform{name, width, height}
{
    m_aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
}

void
Renderer::OnInit() {
    InitVulkan();
    SetupPipeline();
}

void
Renderer::InitVulkan() {
    CreateInstance();
    CreateSurface();
    CreateDevice(VK_QUEUE_GRAPHICS_BIT);
    GetDeviceQueue(
        m_vkparams.Device,
        m_vkparams.GraphicsQueue.FamilyIndex,
        m_vkparams.GraphicsQueue.Handle
    );
    CreateSwapchain(&m_width, &m_height, VKApplication::settings.vsync);
    CreateRenderPass();
    CreateFramebuffers();
    AllocateCommandBuffers();
    CreateSynchronizationObjects();
}

void
Renderer::CreateInstance() {
    // Application Info
    VkApplicationInfo appinfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    appinfo.pApplicationName = GetTitle();
    appinfo.pEngineName = GetTitle();
    appinfo.apiVersion = VK_API_VERSION_1_0;

    // Include generic surface extension that specifies we want to render on the screen
    std::vector<const char*> instanceExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME
    };

    // We also need to do platform specific extensions
//#if defined(Q_PLATFORM_WINDOWS)
//    instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
//#elif defined(Q_PLATFORM_LINUX)
//    instanceExtensions.push_back(VK_KHR_SURFACE_);
//#endif
    
    // Include extension for enabling validation layer
    if (VKApplication::settings.validation) {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // Get supported extension names
    uint32_t extensionCount = 0;
    std::vector<std::string> extensionNames;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (extensionCount > 0) {
        std::vector<VkExtensionProperties> supportedInstanceExtensions(extensionCount);
        if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, &supportedInstanceExtensions.front()) == VK_SUCCESS) {
            for (const VkExtensionProperties& ext : supportedInstanceExtensions) {  
                extensionNames.push_back(ext.extensionName);
            }
        } else {
            printf("vkEnumerateInstanceExtensionProperties did not return VK_SUCCESS\n");
            exit(1);
        }
    }

    // Create vulkan instance
    VkInstanceCreateInfo instanceCreateInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    instanceCreateInfo.pNext = NULL;
    instanceCreateInfo.pApplicationInfo = &appinfo;
}

*/
