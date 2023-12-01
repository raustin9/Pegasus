/**
 * This is the implementation for a Vulkan rendering backend
*/

#include "vulkan_backend.hh"
#include "vulkan_types.inl"
#include "vulkan_utils.hh"

// Initialize the backend for the vulkan renderer
bool
VulkanBackend::Initialize(std::string& name) {
    std::cout << "Vulkan Backend Initialized" << std::endl;
    m_vkallocator = nullptr; // TODO: eventually create custom allocator

    // TODO: Setup the vulkan instance...
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = name.c_str();
    app_info.applicationVersion = VK_MAKE_VERSION(1,0,0);
    app_info.pEngineName = "Pegasus Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1,0,0);

    // Create the vulkan Instance
    VkInstanceCreateInfo instance_info = {};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;

    // TODO: Get platform required extensions
    std::vector<const char*> required_extensions = {};
    required_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    #if defined(P_DEBUG)
    std::cout << "Validation layers are enabled. Enumerating..." << std::endl;
    required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    #endif // debug
    instance_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
    instance_info.ppEnabledExtensionNames = required_extensions.data();

    std::vector<const char*> required_validation_layer_names;

    // If debugging, find the validation layers that we want to use
    #if defined(P_DEBUG)
    required_validation_layer_names.push_back("VK_LAYER_KHRONOS_validation");
    #endif

    instance_info.enabledLayerCount = static_cast<uint32_t>(required_validation_layer_names.size());
    instance_info.ppEnabledLayerNames = required_validation_layer_names.data();

    VK_CHECK(vkCreateInstance(&instance_info, m_vkallocator, &m_vkinstance));
    // vkCreateInstance(&instance_info, m_vkallocator, &m_vkinstance);
    std::cout << "Instance created..." << std::endl;
    
    // Validation Layers

    return true;
}

void
VulkanBackend::Shutdown() {
    vkDestroyInstance(m_vkinstance, m_vkallocator);
}

void
VulkanBackend::Resized(uint32_t width, uint32_t height) {

}

bool
VulkanBackend::BeginFrame(float delta_time) {
    return true;
}

bool 
VulkanBackend::EndFrame(float delta_time) {
    return true;
}