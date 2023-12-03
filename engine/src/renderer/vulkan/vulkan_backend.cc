/**
 * This is the implementation for a Vulkan rendering backend
*/

#include "vulkan_backend.hh"
#include "vulkan_types.hh"

// Initialize the backend for the vulkan renderer
bool
VulkanBackend::Initialize(std::string& name) {
    std::cout << "Vulkan Backend Initialized" << std::endl;
    m_context.allocator = nullptr; // TODO: eventually create custom allocator

    // Initialize all vulkan properties
    if (!create_instance(name.c_str())) {
        return false;
    }

    create_debug_messenger();

    if (!create_surface()) {
        std::cout << "Error: failed to create vulkan surface" << std::endl;
        return false;
    }

    if (!create_device()) {
        std::cout << "Error: Failed to create vulkan device" << std::endl;
        return false;
    }

    // // TODO: Create swapchain
    create_swapchain(
        m_context.framebuffer_width,
        m_context.framebuffer_height,
        m_context.swapchain
    );



    return true;
}

void
VulkanBackend::Shutdown() {
    vkDeviceWaitIdle(m_context.device.logical_device);

    destroy_swapchain();

    destroy_device();

    std::cout << "Destroying surface... ";
    vkDestroySurfaceKHR(m_context.instance, m_context.surface, m_context.allocator);
    std::cout << "Destroyed " << std::endl;

#if defined(P_DEBUG)
    std::cout << "Destroying debug messenger... ";
    PFN_vkDestroyDebugUtilsMessengerEXT func = 
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_context.instance, "vkDestroyDebugUtilsMessengerEXT");
    func(m_context.instance, m_context.debug_messenger, m_context.allocator);
    std::cout << "Destroyed" << std::endl;
#endif // P_DEBUG

    std::cout << "Destroying instance... ";
    vkDestroyInstance(m_context.instance, m_context.allocator);
    std::cout << "Destroyed" << std::endl;
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